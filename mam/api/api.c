/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * MAM is based on an original implementation & specification by apmi.bsu.by
 * [ITSec Lab]
 *
 * Refer to the LICENSE file for licensing information
 */

#include <fcntl.h>
#include <stdlib.h>

#include "common/model/bundle.h"
#include "mam/api/api.h"
#include "mam/mam/mam_channel_t_set.h"
#include "mam/ntru/mam_ntru_pk_t_set.h"
#include "mam/ntru/mam_ntru_sk_t_set.h"
#include "mam/ntru/ntru_types.h"
#include "mam/pb3/pb3.h"
#include "mam/prng/prng.h"
#include "mam/psk/mam_psk_t_set.h"
#include "utils/memset_safe.h"
#include "utils/time.h"

/*
 * Private functions
 */

static void mam_api_bundle_wrap(bundle_transactions_t *const bundle, trit_t const *const address,
                                trit_t const *const tag, trits_t message) {
  iota_transaction_t transaction;
  flex_trit_t buffer[FLEX_TRIT_SIZE_6561];
  trits_t message_part = trits_null();

  // We explicitly set some essence fields to 0 or null trits so that the bundle finalization knows it was intentional
  // and doesn't assert
  transaction_reset(&transaction);
  flex_trits_from_trits(buffer, NUM_TRITS_MESSAGE, address, NUM_TRITS_ADDRESS, NUM_TRITS_ADDRESS);
  transaction_set_address(&transaction, buffer);
  transaction_set_value(&transaction, 0);
  transaction_set_obsolete_tag(&transaction, transaction.essence.obsolete_tag);
  transaction_set_timestamp(&transaction, current_timestamp_ms() / 1000);
  flex_trits_from_trits(buffer, NUM_TRITS_MESSAGE, tag, NUM_TRITS_TAG, NUM_TRITS_TAG);
  transaction_set_tag(&transaction, buffer);

  while (!trits_is_empty(message)) {
    message_part = trits_take_min(message, NUM_TRITS_MESSAGE);
    message = trits_drop_min(message, NUM_TRITS_MESSAGE);
    memset(buffer, FLEX_TRIT_NULL_VALUE, FLEX_TRIT_SIZE_6561);
    flex_trits_from_trits(buffer, NUM_TRITS_MESSAGE, message_part.p + message_part.d, trits_size(message_part),
                          trits_size(message_part));
    transaction_set_message(&transaction, buffer);
    bundle_transactions_add(bundle, &transaction);
  }

  bundle_reset_indexes(bundle);
}

static trits_t mam_api_bundle_unwrap(bundle_transactions_t const *const bundle, trit_t *const msg_trits,
                                     size_t num_trits_in_bundle, size_t start_index) {
  iota_transaction_t *curr_tx = (iota_transaction_t *)utarray_eltptr(bundle, 0);
  trit_t *msg_trits_ptr = msg_trits;
  size_t curr_tx_index = 0;

  BUNDLE_FOREACH(bundle, curr_tx) {
    if (curr_tx_index++ < start_index) {
      continue;
    }
    flex_trits_to_trits(msg_trits_ptr, NUM_TRITS_MESSAGE, transaction_message(curr_tx), NUM_TRITS_MESSAGE,
                        NUM_TRITS_MESSAGE);
    msg_trits_ptr += NUM_TRITS_MESSAGE;
  }

  return trits_from_rep(num_trits_in_bundle, msg_trits);
}

/**
 * Gets a channel from its id
 *
 * @param api - The API [in]
 * @param channel_id - The channel id [in]
 *
 * @return a pointer to the channel or NULL if not found
 */
static mam_channel_t *mam_api_channel_get(mam_api_t const *const api, tryte_t const *const channel_id) {
  trit_t channel_id_trits[MAM_CHANNEL_ID_TRIT_SIZE];
  mam_channel_t_set_entry_t *entry = NULL;
  mam_channel_t_set_entry_t *tmp = NULL;

  if (api == NULL || channel_id == NULL) {
    return NULL;
  }

  trytes_to_trits(channel_id, channel_id_trits, MAM_CHANNEL_ID_TRYTE_SIZE);

  SET_ITER(api->channels, entry, tmp) {
    if (memcmp(trits_begin(mam_channel_id(&entry->value)), channel_id_trits, MAM_CHANNEL_ID_TRIT_SIZE) == 0) {
      return &entry->value;
    }
  }

  return NULL;
}

/**
 * Gets an endpoint from its id
 *
 * @param api - The API [in]
 * @param channel_id - The associated channel id [in]
 * @param endpoint_id - The endpoint id [in]
 *
 * @return a pointer to the endpoint or NULL if not found
 */
static mam_endpoint_t *mam_api_endpoint_get(mam_api_t const *const api, tryte_t const *const channel_id,
                                            tryte_t const *const endpoint_id) {
  trit_t endpoint_id_trits[MAM_ENDPOINT_ID_TRIT_SIZE];
  mam_endpoint_t_set_entry_t *entry = NULL;
  mam_endpoint_t_set_entry_t *tmp = NULL;
  mam_channel_t *parent_channel = mam_api_channel_get(api, channel_id);

  if (endpoint_id == NULL || parent_channel == NULL) {
    return NULL;
  }

  trytes_to_trits(endpoint_id, endpoint_id_trits, MAM_ENDPOINT_ID_TRYTE_SIZE);

  SET_ITER(parent_channel->endpoints, entry, tmp) {
    if (memcmp(trits_begin(mam_endpoint_id(&entry->value)), endpoint_id_trits, MAM_ENDPOINT_ID_TRIT_SIZE) == 0) {
      return &entry->value;
    }
  }

  return NULL;
}

/**
 * Writes MAM header (keyloads (session keys) + potential packet) into a bundle
 *
 * @param api - The API [in,out]
 * @param ch - A known channel [in]
 * @param ep - A known endpoint [in]
 * @param ch1 - A new channel to announce [in]
 * @param ep1 - A new endpoint to announce [in]
 * @param psks - pre shared keys used for encrypting the session keys [in]
 * @param ntru_pks - ntru public keys used for encrypting the session keys [in]
 * @param msg_type_id - The message type [in]
 * @param bundle - The bundle that the packet will be written into [out]
 * @param msg_id - The msg_id (hashed channel_name and message index within the
 *  channel) embedded into transaction's tag (together with packet index to
 * allow Tangle lookup) [out]
 *
 * @return return code
 */
static retcode_t mam_api_bundle_write_header(mam_api_t *const api, tryte_t const *const ch_id,
                                             tryte_t const *const ep_id, tryte_t const *const ch1_id,
                                             tryte_t const *const ep1_id, mam_psk_t_set_t psks,
                                             mam_ntru_pk_t_set_t ntru_pks, mam_msg_type_t msg_type_id,
                                             bundle_transactions_t *const bundle, trit_t *const msg_id) {
  retcode_t ret = RC_OK;
  mam_channel_t *ch = NULL;
  mam_channel_t *ch1 = NULL;
  mam_endpoint_t *ep = NULL;
  mam_endpoint_t *ep1 = NULL;
  mam_msg_write_context_t ctx;
  trit_t tag[NUM_TRITS_TAG];

  if (api == NULL || ch_id == NULL || bundle == NULL || msg_id == NULL) {
    return RC_NULL_PARAM;
  }

  if (bundle_transactions_size(bundle) != 0) {
    return RC_MAM_BUNDLE_NOT_EMPTY;
  }

  if ((ch = mam_api_channel_get(api, ch_id)) == NULL) {
    return RC_MAM_CHANNEL_NOT_FOUND;
  }

  if (ch1_id && (ch1 = mam_api_channel_get(api, ch1_id)) == NULL) {
    return RC_MAM_CHANNEL_NOT_FOUND;
  }

  if (ep_id && (ep = mam_api_endpoint_get(api, ch_id, ep_id)) == NULL) {
    return RC_MAM_ENDPOINT_NOT_FOUND;
  }

  if (ep1_id && (ep1 = mam_api_endpoint_get(api, ch_id, ep1_id)) == NULL) {
    return RC_MAM_ENDPOINT_NOT_FOUND;
  }

  // TODO add a random part
  {
    trits_t msg_id_parts[] = {mam_channel_name(ch), mam_channel_msg_ord(ch)};

    mam_spongos_hashn(&ctx.spongos, 2, msg_id_parts, trits_from_rep(MAM_MSG_ID_SIZE, msg_id));
    add_assign(ch->msg_ord, MAM_CHANNEL_MSG_ORD_SIZE, 1);
    mam_api_write_tag(tag, msg_id, 0);
  }

  {
    trits_t header = trits_null();
    size_t header_size = 0;

    header_size = mam_msg_header_size(ch, ep, ch1, ep1, psks, ntru_pks);
    if (trits_is_null(header = trits_alloc(header_size))) {
      return RC_OOM;
    }
    ERR_BIND_RETURN(mam_msg_write_header(&ctx, &api->prng, ch, ep, ch1, ep1, trits_from_rep(MAM_MSG_ID_SIZE, msg_id),
                                         msg_type_id, psks, ntru_pks, &header),
                    ret);
    header = trits_pickup(header, header_size);
    mam_api_bundle_wrap(bundle, trits_begin(mam_channel_id(ch)), tag, header);
    trits_free(header);
  }

  memcpy(ctx.chid, trits_begin(mam_channel_id(ch)), MAM_CHANNEL_ID_TRIT_SIZE);
  ctx.ord = 1;
  if (ch1) {
    ctx.mss = &ch1->mss;
  } else if (ep1) {
    ctx.mss = &ep1->mss;
  } else if (ep) {
    ctx.mss = &ep->mss;
  } else if (ch) {
    ctx.mss = &ch->mss;
  }

  return trit_t_to_mam_msg_write_context_t_map_add(&api->write_ctxs, msg_id, &ctx);
}

static retcode_t mam_api_bundle_read_packet_from_msg(mam_msg_read_context_t *ctx, trits_t msg, tryte_t **const payload,
                                                     size_t *const payload_size, bool *const is_last_packet) {
  retcode_t ret = RC_OK;
  trits_t payload_trits = trits_null();

  *is_last_packet = false;

  {
    mam_msg_read_context_t rollback_ctx = *ctx;

    if ((ret = mam_msg_read_packet(ctx, &msg, &payload_trits)) != RC_OK) {
      *ctx = rollback_ctx;
      return ret;
    }
  }

  *payload_size = trits_size(payload_trits) / 3;
  *payload = malloc(*payload_size * sizeof(tryte_t));
  trits_to_trytes(payload_trits.p, *payload, *payload_size * 3);
  trits_free(payload_trits);

  // negative ord means it's the last packet, so no need to increment in that
  // case
  if (ctx->ord >= 0) {
    ctx->ord++;
  } else {
    *is_last_packet = true;
  }

  return ret;
}

/*
 * Public functions
 */

retcode_t mam_api_init(mam_api_t *const api, tryte_t const *const mam_seed) {
  retcode_t ret = RC_OK;
  trit_t mam_seed_trits[MAM_PRNG_SECRET_KEY_SIZE];

  if (api == NULL || mam_seed == NULL) {
    return RC_NULL_PARAM;
  }

  trytes_to_trits(mam_seed, mam_seed_trits, MAM_PRNG_SECRET_KEY_SIZE / 3);
  mam_prng_init(&api->prng, trits_from_rep(MAM_PRNG_SECRET_KEY_SIZE, mam_seed_trits));
  memset_safe(mam_seed_trits, MAM_PRNG_SECRET_KEY_SIZE, 0, MAM_PRNG_SECRET_KEY_SIZE);
  api->ntru_sks = NULL;
  api->ntru_pks = NULL;
  api->psks = NULL;
  api->channels = NULL;
  api->channel_ord = 0;
  ERR_BIND_RETURN(
      trit_t_to_mam_msg_write_context_t_map_init(&api->write_ctxs, MAM_MSG_ID_SIZE, sizeof(mam_msg_write_context_t)),
      ret);
  ERR_BIND_RETURN(
      trit_t_to_mam_msg_read_context_t_map_init(&api->read_ctxs, MAM_MSG_ID_SIZE, sizeof(mam_msg_read_context_t)), ret);
  api->trusted_channel_pks = NULL;
  api->trusted_endpoint_pks = NULL;

  return ret;
}

retcode_t mam_api_destroy(mam_api_t *const api) {
  retcode_t ret = RC_OK;

  if (api == NULL) {
    return RC_NULL_PARAM;
  }

  mam_prng_reset(&api->prng);
  mam_ntru_sks_destroy(&api->ntru_sks);
  mam_ntru_pk_t_set_free(&api->ntru_pks);
  mam_psks_destroy(&api->psks);
  mam_channels_destroy(&api->channels);
  if (trit_t_to_mam_msg_write_context_t_map_free(&api->write_ctxs) != RC_OK) {
    // TODO - LOG
  }
  if (trit_t_to_mam_msg_read_context_t_map_free(&api->read_ctxs) != RC_OK) {
    // TODO - LOG
  }
  mam_pk_t_set_free(&api->trusted_channel_pks);
  mam_pk_t_set_free(&api->trusted_endpoint_pks);

  return ret;
}

retcode_t mam_api_add_trusted_channel_pk(mam_api_t *const api, tryte_t const *const pk) {
  mam_pk_t chid;

  trytes_to_trits(pk, chid.key, MAM_CHANNEL_ID_TRYTE_SIZE);

  return mam_pk_t_set_add(&api->trusted_channel_pks, &chid);
}

retcode_t mam_api_add_trusted_endpoint_pk(mam_api_t *const api, tryte_t const *const pk) {
  mam_pk_t epid;

  trytes_to_trits(pk, epid.key, MAM_CHANNEL_ID_TRYTE_SIZE);

  return mam_pk_t_set_add(&api->trusted_endpoint_pks, &epid);
}

retcode_t mam_api_add_ntru_sk(mam_api_t *const api, mam_ntru_sk_t const *const ntru_sk) {
  if (api == NULL || ntru_sk == NULL) {
    return RC_NULL_PARAM;
  }
  return mam_ntru_sk_t_set_add(&api->ntru_sks, ntru_sk);
}

retcode_t mam_api_add_ntru_pk(mam_api_t *const api, mam_ntru_pk_t const *const ntru_pk) {
  if (api == NULL || ntru_pk == NULL) {
    return RC_NULL_PARAM;
  }
  return mam_ntru_pk_t_set_add(&api->ntru_pks, ntru_pk);
}

retcode_t mam_api_add_psk(mam_api_t *const api, mam_psk_t const *const psk) {
  if (api == NULL || psk == NULL) {
    return RC_NULL_PARAM;
  }
  return mam_psk_t_set_add(&api->psks, psk);
}

retcode_t mam_api_channel_create(mam_api_t *const api, size_t const height, tryte_t *const channel_id) {
  retcode_t ret = RC_OK;
  mam_channel_t channel;
  mam_pk_t channel_pk;
  MAM_TRITS_DEF(channel_ord, MAM_CHANNEL_NAME_SIZE);
  channel_ord = MAM_TRITS_INIT(channel_ord, MAM_CHANNEL_NAME_SIZE);

  if (api == NULL || channel_id == NULL) {
    return RC_NULL_PARAM;
  }

  trits_put18(channel_ord, api->channel_ord);
  ERR_BIND_RETURN(mam_channel_create(&api->prng, height, channel_ord, &channel), ret);
  if ((ret = mam_channel_t_set_add(&api->channels, &channel)) != RC_OK) {
    mam_channel_destroy(&channel);
    return ret;
  }
  memcpy(channel_pk.key, trits_begin(mam_channel_id(&channel)), MAM_CHANNEL_ID_TRIT_SIZE);
  ERR_BIND_RETURN(mam_pk_t_set_add(&api->trusted_channel_pks, &channel_pk), ret);
  trits_to_trytes(trits_begin(mam_channel_id(&channel)), channel_id, MAM_CHANNEL_ID_TRIT_SIZE);
  api->channel_ord++;

  return ret;
}

size_t mam_api_channel_remaining_sks(mam_api_t *const api, tryte_t const *const channel_id) {
  return mam_channel_remaining_sks(mam_api_channel_get(api, channel_id));
}

retcode_t mam_api_endpoint_create(mam_api_t *const api, size_t const height, tryte_t const *const channel_id,
                                  tryte_t *const endpoint_id) {
  retcode_t ret = RC_OK;
  mam_channel_t *channel = NULL;
  mam_endpoint_t endpoint;
  mam_pk_t endpoint_pk;
  MAM_TRITS_DEF(endpoint_ord, MAM_ENDPOINT_NAME_SIZE);
  endpoint_ord = MAM_TRITS_INIT(endpoint_ord, MAM_ENDPOINT_NAME_SIZE);

  if (api == NULL || channel_id == NULL || endpoint_id == NULL) {
    return RC_NULL_PARAM;
  }

  if ((channel = mam_api_channel_get(api, channel_id)) == NULL) {
    return RC_MAM_CHANNEL_NOT_FOUND;
  }

  trits_put18(endpoint_ord, channel->endpoint_ord);
  ERR_BIND_RETURN(mam_endpoint_create(&api->prng, height, mam_channel_name_size(channel), mam_channel_name(channel),
                                      endpoint_ord, &endpoint),
                  ret);
  if ((ret = mam_endpoint_t_set_add(&channel->endpoints, &endpoint)) != RC_OK) {
    mam_endpoint_destroy(&endpoint);
    return ret;
  }
  memcpy(endpoint_pk.key, trits_begin(mam_endpoint_id(&endpoint)), MAM_ENDPOINT_ID_TRIT_SIZE);
  ERR_BIND_RETURN(mam_pk_t_set_add(&api->trusted_endpoint_pks, &endpoint_pk), ret);
  trits_to_trytes(trits_begin(mam_endpoint_id(&endpoint)), endpoint_id, MAM_ENDPOINT_ID_TRIT_SIZE);
  channel->endpoint_ord++;

  return ret;
}

size_t mam_api_endpoint_remaining_sks(mam_api_t *const api, tryte_t const *const channel_id,
                                      tryte_t const *const endpoint_id) {
  return mam_endpoint_remaining_sks(mam_api_endpoint_get(api, channel_id, endpoint_id));
}

void mam_api_write_tag(trit_t *const tag, trit_t const *const msg_id, trint18_t const ord) {
  memcpy(tag, msg_id, MAM_MSG_ID_SIZE);
  trits_put18(trits_from_rep(18, tag + MAM_MSG_ID_SIZE), ord);
}

retcode_t mam_api_bundle_write_header_on_channel(mam_api_t *const api, tryte_t const *const ch_id, mam_psk_t_set_t psks,
                                                 mam_ntru_pk_t_set_t ntru_pks, bundle_transactions_t *const bundle,
                                                 trit_t *const msg_id) {
  return mam_api_bundle_write_header(api, ch_id, NULL, NULL, NULL, psks, ntru_pks, MAM_MSG_TYPE_UNSTRUCTURED, bundle,
                                     msg_id);
}

retcode_t mam_api_bundle_write_header_on_endpoint(mam_api_t *const api, tryte_t const *const ch_id,
                                                  tryte_t const *const ep_id, mam_psk_t_set_t psks,
                                                  mam_ntru_pk_t_set_t ntru_pks, bundle_transactions_t *const bundle,
                                                  trit_t *const msg_id) {
  return mam_api_bundle_write_header(api, ch_id, ep_id, NULL, NULL, psks, ntru_pks, MAM_MSG_TYPE_UNSTRUCTURED, bundle,
                                     msg_id);
}

retcode_t mam_api_bundle_announce_channel(mam_api_t *const api, tryte_t const *const ch_id, tryte_t const *const ch1_id,
                                          mam_psk_t_set_t psks, mam_ntru_pk_t_set_t ntru_pks,
                                          bundle_transactions_t *const bundle, trit_t *const msg_id) {
  return mam_api_bundle_write_header(api, ch_id, NULL, ch1_id, NULL, psks, ntru_pks, MAM_MSG_TYPE_UNSTRUCTURED, bundle,
                                     msg_id);
}

retcode_t mam_api_bundle_announce_endpoint(mam_api_t *const api, tryte_t const *const ch_id,
                                           tryte_t const *const ep1_id, mam_psk_t_set_t psks,
                                           mam_ntru_pk_t_set_t ntru_pks, bundle_transactions_t *const bundle,
                                           trit_t *const msg_id) {
  return mam_api_bundle_write_header(api, ch_id, NULL, NULL, ep1_id, psks, ntru_pks, MAM_MSG_TYPE_UNSTRUCTURED, bundle,
                                     msg_id);
}

retcode_t mam_api_bundle_write_packet(mam_api_t *const api, trit_t const *const msg_id, tryte_t const *const payload,
                                      size_t const payload_size, mam_msg_checksum_t checksum, bool is_last_packet,
                                      bundle_transactions_t *const bundle) {
  retcode_t ret = RC_OK;
  mam_msg_write_context_t *ctx = NULL;
  trit_t_to_mam_msg_write_context_t_map_entry_t *entry = NULL;
  trit_t tag[NUM_TRITS_TAG];
  trits_t packet = trits_null();
  trits_t payload_trits = trits_null();
  size_t packet_size = 0;

  if (api == NULL || msg_id == NULL || payload == NULL || bundle == NULL) {
    return RC_NULL_PARAM;
  }

  if (!trit_t_to_mam_msg_write_context_t_map_find(api->write_ctxs, msg_id, &entry) || entry == NULL) {
    return RC_MAM_MESSAGE_NOT_FOUND;
  }
  ctx = entry->value;

  if (trits_is_null(payload_trits = trits_alloc(payload_size * 3))) {
    ret = RC_OOM;
    goto done;
  }
  trits_from_str(payload_trits, (char const *)payload);

  packet_size = mam_msg_packet_size(checksum, ctx->mss, payload_size * 3);
  if (trits_is_null(packet = trits_alloc(packet_size))) {
    ret = RC_OOM;
    goto done;
  }

  if (is_last_packet) {
    ctx->ord = -ctx->ord;
  }

  ERR_BIND_GOTO(mam_msg_write_packet(ctx, checksum, payload_trits, &packet), ret, done);
  packet = trits_pickup(packet, packet_size);
  mam_api_write_tag(tag, msg_id, ctx->ord);

  if (!is_last_packet) {
    ctx->ord++;
  }
  mam_api_bundle_wrap(bundle, ctx->chid, tag, packet);

  if (is_last_packet) {
    if (!trit_t_to_mam_msg_write_context_t_map_remove(&api->write_ctxs, msg_id)) {
      ret = RC_MAM_SEND_CTX_NOT_FOUND;
    }
  }

done:
  trits_free(packet);
  trits_free(payload_trits);

  return ret;
}

retcode_t mam_api_bundle_read(mam_api_t *const api, bundle_transactions_t const *const bundle, tryte_t **const payload,
                              size_t *const payload_size, bool *const is_last_packet) {
  retcode_t ret = RC_OK;
  trit_t tag[NUM_TRITS_TAG];
  trit_t *const msg_id = tag;
  iota_transaction_t *curr_tx = NULL;

  MAM_ASSERT(payload && *payload == NULL && payload_size);

  *is_last_packet = false;

  // Flatten flex_trits encoded in transaction sig_or_fragment field
  // into a single long trits_t data structure
  size_t num_trits_in_bundle = bundle_transactions_size(bundle) * NUM_TRITS_MESSAGE;
  trit_t msg_trits[num_trits_in_bundle];
  trits_t msg = mam_api_bundle_unwrap(bundle, msg_trits, num_trits_in_bundle, 0);

  curr_tx = (iota_transaction_t *)utarray_eltptr(bundle, 0);

  flex_trits_to_trits(tag, NUM_TRITS_TAG, transaction_tag(curr_tx), NUM_TRITS_TAG, NUM_TRITS_TAG);

  trint18_t ord = trits_get18(trits_from_rep(MAM_MSG_ORD_SIZE, tag + MAM_MSG_ID_SIZE));
  // If header
  if (ord == 0) {
    mam_msg_read_context_t ctx;

    if (trit_t_to_mam_msg_read_context_t_map_contains(api->read_ctxs, msg_id)) {
      return RC_OK;
    }

    flex_trits_to_trits(ctx.pk.key, NUM_TRITS_ADDRESS, transaction_address(curr_tx), NUM_TRITS_ADDRESS,
                        NUM_TRITS_ADDRESS);

    ctx.ord = 1;

    ERR_BIND_RETURN(mam_msg_read_header(&ctx, &msg, api->psks, api->ntru_sks, trits_from_rep(MAM_MSG_ID_SIZE, tag),
                                        &api->trusted_channel_pks, &api->trusted_endpoint_pks),
                    ret);

    size_t packet_index = msg.d / NUM_TRITS_MESSAGE + 1;
    if (packet_index < bundle_transactions_size(bundle)) {
      curr_tx = (iota_transaction_t *)utarray_eltptr(bundle, packet_index);

      flex_trits_to_trits(tag, NUM_TRITS_TAG, transaction_tag(curr_tx), NUM_TRITS_TAG, NUM_TRITS_TAG);

      ord = trits_get18(trits_from_rep(MAM_MSG_ORD_SIZE, tag + MAM_MSG_ID_SIZE));

      if (ord < 0) {
        ctx.ord = ord;
      }

      trits_advance(&msg, NUM_TRITS_MESSAGE - msg.d % NUM_TRITS_MESSAGE);
      ERR_BIND_RETURN(mam_api_bundle_read_packet_from_msg(&ctx, msg, payload, payload_size, is_last_packet), ret);
    }

    return trit_t_to_mam_msg_read_context_t_map_add(&api->read_ctxs, msg_id, &ctx);
  }
  // Else packet
  else {
    mam_msg_read_context_t *ctx = NULL;
    trit_t_to_mam_msg_read_context_t_map_entry_t *entry = NULL;

    if (!trit_t_to_mam_msg_read_context_t_map_find(api->read_ctxs, msg_id, &entry) || entry == NULL) {
      return RC_MAM_MESSAGE_NOT_FOUND;
    }

    ctx = entry->value;

    if (ord != ctx->ord && -ord != ctx->ord) {
      return RC_MAM_BAD_PACKET_ORD;
    }

    if (ord < 0) {
      ctx->ord = -ctx->ord;
    }

    ERR_BIND_RETURN(mam_api_bundle_read_packet_from_msg(ctx, msg, payload, payload_size, is_last_packet), ret);
    if (*is_last_packet) {
      if (!trit_t_to_mam_msg_read_context_t_map_remove(&api->read_ctxs, msg_id)) {
        return RC_MAM_RECV_CTX_NOT_FOUND;
      }
    }
  }

  return RC_OK;
}

size_t mam_api_write_ctx_map_serialized_size(trit_t_to_mam_msg_write_context_t_map_t const *const map) {
  trit_t_to_mam_msg_write_context_t_map_entry_t *curr_entry = NULL;
  trit_t_to_mam_msg_write_context_t_map_entry_t *tmp_entry = NULL;
  size_t serialized_size = pb3_sizeof_size_t(trit_t_to_mam_msg_write_context_t_map_size(*map));

  HASH_ITER(hh, map->map, curr_entry, tmp_entry) {
    serialized_size += (MAM_MSG_ID_SIZE + mam_msg_write_ctx_serialized_size(curr_entry->value));
  }

  return serialized_size;
}

void mam_api_write_ctx_map_serialize(trit_t_to_mam_msg_write_context_t_map_t const *const map, trits_t *const buffer) {
  trit_t_to_mam_msg_write_context_t_map_entry_t *curr_entry = NULL;
  trit_t_to_mam_msg_write_context_t_map_entry_t *tmp_entry = NULL;

  pb3_encode_size_t(trit_t_to_mam_msg_write_context_t_map_size(*map), buffer);

  HASH_ITER(hh, map->map, curr_entry, tmp_entry) {
    pb3_encode_ntrytes(trits_from_rep(MAM_MSG_ID_SIZE, curr_entry->key), buffer);
    mam_msg_write_ctx_serialize(curr_entry->value, buffer);
  }
}

retcode_t mam_api_write_ctx_map_deserialize(trits_t *const buffer, trit_t_to_mam_msg_write_context_t_map_t *const map,
                                            mam_channel_t_set_t const channels) {
  retcode_t ret;
  mam_msg_write_context_t ctx;
  trit_t msg_id[MAM_MSG_ID_SIZE];
  size_t num_ctxs = 0;

  pb3_decode_size_t(&num_ctxs, buffer);

  for (size_t i = 0; i < num_ctxs; ++i) {
    ERR_BIND_RETURN(pb3_decode_ntrytes(trits_from_rep(MAM_MSG_ID_SIZE, msg_id), buffer), ret);
    ERR_BIND_RETURN(mam_msg_write_ctx_deserialize(buffer, &ctx, channels), ret);
    ERR_BIND_RETURN(trit_t_to_mam_msg_write_context_t_map_add(map, msg_id, &ctx), ret);
  }

  return RC_OK;
}

size_t mam_api_read_ctx_map_serialized_size(trit_t_to_mam_msg_read_context_t_map_t const *const map) {
  trit_t_to_mam_msg_read_context_t_map_entry_t *curr_entry = NULL;
  trit_t_to_mam_msg_read_context_t_map_entry_t *tmp_entry = NULL;
  size_t serialized_size = pb3_sizeof_size_t(trit_t_to_mam_msg_read_context_t_map_size(*map));

  HASH_ITER(hh, map->map, curr_entry, tmp_entry) {
    serialized_size += (MAM_MSG_ID_SIZE + mam_msg_read_ctx_serialized_size(curr_entry->value));
  }
  return serialized_size;
}

void mam_api_read_ctx_map_serialize(trit_t_to_mam_msg_read_context_t_map_t const *const map, trits_t *const buffer) {
  trit_t_to_mam_msg_read_context_t_map_entry_t *curr_entry = NULL;
  trit_t_to_mam_msg_read_context_t_map_entry_t *tmp_entry = NULL;

  pb3_encode_size_t(trit_t_to_mam_msg_read_context_t_map_size(*map), buffer);

  HASH_ITER(hh, map->map, curr_entry, tmp_entry) {
    pb3_encode_ntrytes(trits_from_rep(MAM_MSG_ID_SIZE, curr_entry->key), buffer);
    mam_msg_read_ctx_serialize(curr_entry->value, buffer);
  }
}

retcode_t mam_api_read_ctx_map_deserialize(trits_t *const buffer, trit_t_to_mam_msg_read_context_t_map_t *const map) {
  retcode_t ret;
  mam_msg_read_context_t ctx;
  trit_t msg_id[MAM_MSG_ID_SIZE];
  size_t num_ctxs = 0;

  pb3_decode_size_t(&num_ctxs, buffer);

  for (size_t i = 0; i < num_ctxs; ++i) {
    pb3_decode_ntrytes(trits_from_rep(MAM_MSG_ID_SIZE, msg_id), buffer);
    ERR_BIND_RETURN(mam_msg_read_ctx_deserialize(buffer, &ctx), ret);
    ERR_BIND_RETURN(trit_t_to_mam_msg_read_context_t_map_add(map, msg_id, &ctx), ret);
  }

  return RC_OK;
}

static size_t mam_pks_serialized_size(mam_pk_t_set_t const pks) {
  size_t pks_size = mam_pk_t_set_size(pks);

  return pb3_sizeof_size_t(pks_size) + pks_size * MAM_CHANNEL_ID_TRIT_SIZE;
}

static retcode_t mam_pks_serialize(mam_pk_t_set_t const pks, trits_t *const trits) {
  mam_pk_t_set_entry_t *entry = NULL;
  mam_pk_t_set_entry_t *tmp = NULL;

  if (trits == NULL) {
    return RC_NULL_PARAM;
  }

  pb3_encode_size_t(mam_pk_t_set_size(pks), trits);

  SET_ITER(pks, entry, tmp) { pb3_encode_ntrytes(trits_from_rep(MAM_CHANNEL_ID_TRIT_SIZE, entry->value.key), trits); }

  return RC_OK;
}

retcode_t mam_pks_deserialize(trits_t *const trits, mam_pk_t_set_t *const pks) {
  retcode_t ret = RC_OK;
  size_t pks_size = 0;
  mam_pk_t pk;

  if (trits == NULL || pks == NULL) {
    return RC_NULL_PARAM;
  }

  ERR_BIND_RETURN(pb3_decode_size_t(&pks_size, trits), ret);

  for (size_t i = 0; i < pks_size; i++) {
    ERR_BIND_RETURN(pb3_decode_ntrytes(trits_from_rep(MAM_CHANNEL_ID_TRIT_SIZE, pk.key), trits), ret);
    ERR_BIND_RETURN(mam_pk_t_set_add(pks, &pk), ret);
  }

  return ret;
}

size_t mam_api_serialized_size(mam_api_t const *const api) {
  return mam_prng_serialized_size() + mam_ntru_sks_serialized_size(api->ntru_sks) +
         mam_ntru_pks_serialized_size(api->ntru_pks) + mam_psks_serialized_size(api->psks) +
         mam_channels_serialized_size(api->channels) + pb3_sizeof_longtrint() +
         mam_api_write_ctx_map_serialized_size(&api->write_ctxs) +
         mam_api_read_ctx_map_serialized_size(&api->read_ctxs) + mam_pks_serialized_size(api->trusted_channel_pks) +
         mam_pks_serialized_size(api->trusted_endpoint_pks);
}

void mam_api_serialize(mam_api_t const *const api, trit_t *const buffer, tryte_t const *const encr_key_trytes,
                       size_t encr_key_trytes_size) {
  size_t trits_buffer_size = mam_api_serialized_size(api);
  mam_spongos_t spongos;

  trits_t trits = trits_from_rep(trits_buffer_size, buffer);

  mam_prng_serialize(&api->prng, &trits);
  mam_ntru_sks_serialize(api->ntru_sks, &trits);
  mam_ntru_pks_serialize(api->ntru_pks, &trits);
  mam_psks_serialize(api->psks, &trits);
  mam_channels_serialize(api->channels, &trits);
  pb3_encode_longtrint(api->channel_ord, &trits);
  mam_api_write_ctx_map_serialize(&api->write_ctxs, &trits);
  mam_api_read_ctx_map_serialize(&api->read_ctxs, &trits);
  mam_pks_serialize(api->trusted_channel_pks, &trits);
  mam_pks_serialize(api->trusted_endpoint_pks, &trits);

  if (encr_key_trytes != NULL && encr_key_trytes_size) {
    MAM_TRITS_DEF(encrypt_key_trits, encr_key_trytes_size * NUMBER_OF_TRITS_IN_A_TRYTE);
    encrypt_key_trits = MAM_TRITS_INIT(encrypt_key_trits, encr_key_trytes_size * NUMBER_OF_TRITS_IN_A_TRYTE);
    trytes_to_trits(encr_key_trytes, trits_begin(encrypt_key_trits), encr_key_trytes_size);
    trits = trits_pickup_all(trits);
    mam_spongos_init(&spongos);
    mam_spongos_absorb(&spongos, encrypt_key_trits);
    mam_spongos_commit(&spongos);
    mam_spongos_encr(&spongos, trits, trits);
  }
}

retcode_t mam_api_deserialize(trit_t const *const buffer, size_t const buffer_size, mam_api_t *const api,
                              tryte_t const *const decr_key_trytes, size_t decr_key_trytes_size) {
  retcode_t ret;
  trits_t trits = trits_from_rep(buffer_size, buffer);
  mam_spongos_t spongos;

  if (decr_key_trytes != NULL && decr_key_trytes_size > 0) {
    MAM_TRITS_DEF(decrypt_key_trits, decr_key_trytes_size * NUMBER_OF_TRITS_IN_A_TRYTE);
    decrypt_key_trits = MAM_TRITS_INIT(decrypt_key_trits, decr_key_trytes_size * NUMBER_OF_TRITS_IN_A_TRYTE);
    trytes_to_trits(decr_key_trytes, trits_begin(decrypt_key_trits), decr_key_trytes_size);
    trits = trits_pickup_all(trits);
    mam_spongos_init(&spongos);
    mam_spongos_absorb(&spongos, decrypt_key_trits);
    mam_spongos_commit(&spongos);
    mam_spongos_decr(&spongos, trits, trits);
  }

  // TODO should we use mam_api_init here ?
  api->ntru_sks = NULL;
  api->ntru_pks = NULL;
  api->psks = NULL;
  api->channels = NULL;
  trit_t_to_mam_msg_write_context_t_map_init(&api->write_ctxs, MAM_MSG_ID_SIZE, sizeof(mam_msg_write_context_t));
  trit_t_to_mam_msg_read_context_t_map_init(&api->read_ctxs, MAM_MSG_ID_SIZE, sizeof(mam_msg_read_context_t));
  api->trusted_channel_pks = NULL;
  api->trusted_endpoint_pks = NULL;

  ERR_BIND_RETURN(mam_prng_deserialize(&trits, &api->prng), ret);
  ERR_BIND_RETURN(mam_ntru_sks_deserialize(&trits, &api->ntru_sks), ret);
  ERR_BIND_RETURN(mam_ntru_pks_deserialize(&trits, &api->ntru_pks), ret);
  ERR_BIND_RETURN(mam_psks_deserialize(&trits, &api->psks), ret);
  ERR_BIND_RETURN(mam_channels_deserialize(&trits, &api->prng, &api->channels), ret);
  ERR_BIND_RETURN(pb3_decode_longtrint(&api->channel_ord, &trits), ret);
  ERR_BIND_RETURN(mam_api_write_ctx_map_deserialize(&trits, &api->write_ctxs, api->channels), ret);
  ERR_BIND_RETURN(mam_api_read_ctx_map_deserialize(&trits, &api->read_ctxs), ret);
  ERR_BIND_RETURN(mam_pks_deserialize(&trits, &api->trusted_channel_pks), ret);
  ERR_BIND_RETURN(mam_pks_deserialize(&trits, &api->trusted_endpoint_pks), ret);

  return RC_OK;
}

retcode_t mam_api_save(mam_api_t const *const api, char const *const filename, tryte_t const *const encr_key_trytes,
                       size_t encr_key_trytes_size) {
  retcode_t ret = RC_OK;
  size_t trits_buffer_size = 0;
  trit_t *trits_buffer = NULL;

  size_t bytes_size = 0;
  byte_t *bytes = NULL;
  FILE *file = NULL;

  if (api == NULL || filename == NULL) {
    return RC_NULL_PARAM;
  }

  trits_buffer_size = mam_api_serialized_size(api);

  if ((trits_buffer = malloc(trits_buffer_size * sizeof(trit_t))) == NULL) {
    ret = RC_OOM;
    goto done;
  }

  mam_api_serialize(api, trits_buffer, encr_key_trytes, encr_key_trytes_size);

  bytes_size = MIN_BYTES(trits_buffer_size);
  if ((bytes = (byte_t *)malloc(bytes_size)) == NULL) {
    ret = RC_OOM;
    goto done;
  }
  trits_to_bytes(trits_buffer, bytes, trits_buffer_size);
  if ((file = fopen(filename, "w")) == NULL) {
    ret = RC_UTILS_FAILED_TO_OPEN_FILE;
    goto done;
  }
  if (fwrite(bytes, sizeof(byte_t), bytes_size, file) != bytes_size) {
    ret = RC_UTILS_FAILED_WRITE_FILE;
    goto done;
  }

done:
  free(trits_buffer);
  free(bytes);
  fclose(file);

  return ret;
}

retcode_t mam_api_load(char const *const filename, mam_api_t *const api, tryte_t const *const decr_key_trytes,
                       size_t decr_key_trytes_size) {
  retcode_t ret = RC_OK;
  size_t trits_buffer_size = 0;
  trit_t *trits_buffer = NULL;
  size_t bytes_size = 0;
  byte_t *bytes = NULL;
  FILE *file = NULL;

  if (filename == NULL || api == NULL) {
    return RC_NULL_PARAM;
  }

  if ((file = fopen(filename, "r")) == NULL) {
    return RC_UTILS_FAILED_TO_OPEN_FILE;
  }

  fseek(file, 0, SEEK_END);
  bytes_size = ftell(file);
  fseek(file, 0, SEEK_SET);

  trits_buffer_size = bytes_size * NUMBER_OF_TRITS_IN_A_BYTE;

  if ((bytes = (byte_t *)malloc(bytes_size)) == NULL) {
    ret = RC_OOM;
    goto done;
  }

  if (fread(bytes, sizeof(byte_t), bytes_size, file) != bytes_size) {
    ret = RC_UTILS_FAILED_READ_FILE;
    goto done;
  }

  if ((trits_buffer = malloc(trits_buffer_size * sizeof(trit_t))) == NULL) {
    ret = RC_OOM;
    goto done;
  }

  bytes_to_trits(bytes, bytes_size, trits_buffer, bytes_size * NUMBER_OF_TRITS_IN_A_BYTE);
  if ((ret = mam_api_deserialize(trits_buffer, trits_buffer_size, api, decr_key_trytes, decr_key_trytes_size)) !=
      RC_OK) {
    goto done;
  }

done:
  free(bytes);
  free(trits_buffer);
  fclose(file);

  return ret;
}
