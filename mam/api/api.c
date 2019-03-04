/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * MAM is based on an original implementation & specification by apmi.bsu.by
 * [ITSec Lab]
 *
 * Refer to the LICENSE file for licensing information
 */

#include <stdlib.h>

#include "common/model/bundle.h"

#include "mam/api/api.h"
#include "mam/mam/mam_channel_t_set.h"
#include "mam/ntru/mam_ntru_pk_t_set.h"
#include "mam/ntru/mam_ntru_sk_t_set.h"
#include "mam/ntru/ntru_types.h"
#include "mam/prng/prng.h"
#include "mam/psk/mam_psk_t_set.h"

#include "mam/pb3/pb3.h"
#include "utils/time.h"

/*
 * Private functions
 */

static void mam_api_bundle_wrap(bundle_transactions_t *const bundle,
                                trit_t const *const address,
                                trit_t const *const tag, trits_t message) {
  iota_transaction_t transaction;
  flex_trit_t buffer[FLEX_TRIT_SIZE_6561];
  trits_t message_part = trits_null();

  transaction_reset(&transaction);
  flex_trits_from_trits(buffer, NUM_TRITS_MESSAGE, address, NUM_TRITS_ADDRESS,
                        NUM_TRITS_ADDRESS);
  transaction_set_address(&transaction, buffer);
  transaction_set_value(&transaction, 0);
  transaction_set_obsolete_tag(&transaction,
                               transaction.data.signature_or_message);
  transaction_set_timestamp(&transaction, current_timestamp_ms() / 1000);
  flex_trits_from_trits(buffer, NUM_TRITS_MESSAGE, tag, NUM_TRITS_TAG,
                        NUM_TRITS_TAG);
  transaction_set_tag(&transaction, buffer);

  while (!trits_is_empty(message)) {
    message_part = trits_take_min(message, NUM_TRITS_SIGNATURE);
    message = trits_drop_min(message, NUM_TRITS_SIGNATURE);
    memset(buffer, FLEX_TRIT_NULL_VALUE, FLEX_TRIT_SIZE_6561);
    flex_trits_from_trits(buffer, NUM_TRITS_SIGNATURE,
                          message_part.p + message_part.d,
                          trits_size(message_part), trits_size(message_part));
    transaction_set_message(&transaction, buffer);
    bundle_transactions_add(bundle, &transaction);
  }

  bundle_reset_indexes(bundle);
}

static trits_t mam_api_bundle_unwrap(bundle_transactions_t const *const bundle,
                                     trit_t const *msg_trits,
                                     size_t num_trits_in_bundle,
                                     size_t start_index) {
  iota_transaction_t *curr_tx = (iota_transaction_t *)utarray_eltptr(bundle, 0);
  trit_t *msg_trits_ptr = msg_trits;
  size_t curr_tx_index = 0;

  BUNDLE_FOREACH(bundle, curr_tx) {
    if (curr_tx_index++ < start_index) {
      continue;
    }
    flex_trits_to_trits(msg_trits_ptr, NUM_TRITS_SIGNATURE,
                        transaction_message(curr_tx), NUM_TRITS_SIGNATURE,
                        NUM_TRITS_SIGNATURE);
    msg_trits_ptr += NUM_TRITS_SIGNATURE;
  }

  return trits_from_rep(num_trits_in_bundle, msg_trits);
}

/*
 * Public functions
 */

retcode_t mam_api_init(mam_api_t *const api, tryte_t const *const mam_seed) {
  retcode_t ret = RC_OK;
  trit_t mam_seed_trits[MAM_PRNG_KEY_SIZE];

  if (api == NULL || mam_seed == NULL) {
    return RC_NULL_PARAM;
  }

  trytes_to_trits(mam_seed, mam_seed_trits, MAM_PRNG_KEY_SIZE / 3);
  mam_prng_init(&api->prng, trits_from_rep(MAM_PRNG_KEY_SIZE, mam_seed_trits));
  // TODO use safe memset
  memset(mam_seed_trits, 0, MAM_PRNG_KEY_SIZE);
  api->ntru_sks = NULL;
  api->ntru_pks = NULL;
  api->psks = NULL;
  api->channels = NULL;

  ERR_BIND_RETURN(trit_t_to_mam_msg_send_context_t_map_init(&api->send_ctxs,
                                                            MAM_MSG_ID_SIZE),
                  ret);
  ERR_BIND_RETURN(trit_t_to_mam_msg_recv_context_t_map_init(&api->recv_ctxs,
                                                            MAM_MSG_ID_SIZE),
                  ret);

  return ret;
}

retcode_t mam_api_destroy(mam_api_t *const api) {
  retcode_t ret = RC_OK;

  if (api == NULL) {
    return RC_NULL_PARAM;
  }

  mam_prng_destroy(&api->prng);
  mam_ntru_sk_t_set_free(&api->ntru_sks);
  mam_ntru_pk_t_set_free(&api->ntru_pks);
  mam_psk_t_set_free(&api->psks);
  mam_channel_t_set_free(&api->channels);
  ret = trit_t_to_mam_msg_send_context_t_map_free(&api->send_ctxs);
  if (ret != RC_OK) {
    // TODO - LOG
  }
  ret = trit_t_to_mam_msg_recv_context_t_map_free(&api->recv_ctxs);
  if (ret != RC_OK) {
    // TODO - LOG
  }

  return ret;
}

retcode_t mam_api_add_ntru_sk(mam_api_t *const api,
                              mam_ntru_sk_t const *const ntru_sk) {
  if (api == NULL || ntru_sk == NULL) {
    return RC_NULL_PARAM;
  }
  return mam_ntru_sk_t_set_add(&api->ntru_sks, ntru_sk);
}

retcode_t mam_api_add_ntru_pk(mam_api_t *const api,
                              mam_ntru_pk_t const *const ntru_pk) {
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

retcode_t mam_api_add_channel(mam_api_t *const api,
                              mam_channel_t const *const ch) {
  if (api == NULL || ch == NULL) {
    return RC_NULL_PARAM;
  }
  return mam_channel_t_set_add(&api->channels, ch);
}

void mam_api_tag(trit_t *const tag, trit_t const *const msg_id,
                 trint18_t const ord) {
  memcpy(tag, msg_id, MAM_MSG_ID_SIZE);
  trits_put18(trits_from_rep(18, tag + MAM_MSG_ID_SIZE), ord);
}

retcode_t mam_api_bundle_write_header(
    mam_api_t *const api, mam_channel_t *const ch,
    mam_endpoint_t const *const ep, mam_channel_t const *const ch1,
    mam_endpoint_t const *const ep1, mam_psk_t_set_t psks,
    mam_ntru_pk_t_set_t ntru_pks, trint9_t msg_type_id,
    bundle_transactions_t *const bundle, trit_t *const msg_id) {
  retcode_t ret = RC_OK;
  mam_msg_send_context_t ctx;
  trit_t tag[NUM_TRITS_TAG];

  if (api == NULL || ch == NULL || bundle == NULL || msg_id == NULL) {
    return RC_NULL_PARAM;
  }

  if (bundle_transactions_size(bundle) != 0) {
    return RC_MAM_BUNDLE_NOT_EMPTY;
  }

  // TODO add a random part
  {
    trits_t msg_id_parts[] = {mam_channel_name(ch), mam_channel_msg_ord(ch)};

    mam_spongos_hashn(&ctx.spongos, 2, msg_id_parts,
                      trits_from_rep(MAM_MSG_ID_SIZE, msg_id));
    add_assign(ch->msg_ord, MAM_MSG_ID_SIZE, 1);
    mam_api_tag(tag, msg_id, 0);
  }

  {
    trits_t header = trits_null();
    size_t header_size = 0;

    header_size = mam_msg_send_size(ch, ep, ch1, ep1, psks, ntru_pks);
    if (trits_is_null(header = trits_alloc(header_size))) {
      return RC_OOM;
    }
    mam_msg_send(&ctx, &api->prng, ch, ep, ch1, ep1,
                 trits_from_rep(MAM_MSG_ID_SIZE, msg_id), msg_type_id, psks,
                 ntru_pks, &header);
    header = trits_pickup(header, header_size);
    mam_api_bundle_wrap(bundle, mam_channel_id(ch).p, tag, header);
    trits_free(header);
  }

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

  return trit_t_to_mam_msg_send_context_t_map_add(&api->send_ctxs, msg_id, ctx);
}

retcode_t mam_api_bundle_write_packet(
    mam_api_t *const api, mam_channel_t *const ch, trit_t *const msg_id,
    tryte_t const *const payload, size_t const payload_size,
    mam_msg_checksum_t checksum, bundle_transactions_t *const bundle) {
  mam_msg_send_context_t *ctx = NULL;
  trit_t_to_mam_msg_send_context_t_map_entry_t *entry = NULL;

  if (api == NULL || msg_id == NULL || payload == NULL || bundle == NULL) {
    return RC_NULL_PARAM;
  }

  if (!trit_t_to_mam_msg_send_context_t_map_find(&api->send_ctxs, msg_id,
                                                 &entry) ||
      entry == NULL) {
    return RC_MAM_MESSAGE_NOT_FOUND;
  }
  ctx = &entry->value;

  {
    trit_t tag[NUM_TRITS_TAG];
    trits_t packet = trits_null();
    size_t packet_size = 0;
    MAM_TRITS_DEF0(payload_trits, payload_size * 3);
    payload_trits = MAM_TRITS_INIT(payload_trits, payload_size * 3);
    trits_from_str(payload_trits, payload);

    packet_size =
        mam_msg_send_packet_size(checksum, ctx->mss, payload_size * 3);
    if (trits_is_null(packet = trits_alloc(packet_size))) {
      return RC_OOM;
    }

    mam_msg_send_packet(ctx, checksum, payload_trits, &packet);
    packet = trits_pickup(packet, packet_size);
    // TODO negate if last packet
    mam_api_tag(tag, msg_id, ctx->ord++);
    mam_api_bundle_wrap(bundle, mam_channel_id(ch).p, tag, packet);
    trits_free(packet);
  }

  // TODO if last remove pending state

  return RC_OK;
}

bool mam_api_bundle_contains_header(bundle_transactions_t const *const bundle) {
  trit_t tag[NUM_TRITS_TAG];
  iota_transaction_t *tx = (iota_transaction_t *)utarray_front(bundle);

  flex_trits_to_trits(tag, NUM_TRITS_TAG, transaction_tag(tx), NUM_TRITS_TAG,
                      NUM_TRITS_TAG);
  return trits_get18(trits_from_rep(18, tag + MAM_MSG_ID_SIZE)) == 0;
}

static retcode_t mam_api_bundle_read_packet_from_msg(
    mam_api_t *const api, mam_msg_recv_context_t *ctx, trits_t msg,
    tryte_t **const payload, size_t *const payload_size) {
  retcode_t err;
  trits_t payload_trits = trits_null();

  ERR_BIND_RETURN(mam_msg_recv_packet(ctx, &msg, &payload_trits), err);
  *payload_size = trits_size(payload_trits) / 3;
  *payload = malloc(*payload_size * sizeof(tryte_t));
  trits_to_trytes(payload_trits.p, *payload, *payload_size * 3);
  trits_free(payload_trits);
  ctx->ord++;

  return RC_OK;
}

retcode_t mam_api_bundle_read_msg(mam_api_t *const api,
                                  bundle_transactions_t const *const bundle,
                                  tryte_t **const payload,
                                  size_t *const payload_size) {
  retcode_t err = RC_OK;
  trit_t msg_id[MAM_MSG_ID_SIZE];

  MAM_ASSERT(payload && *payload == NULL && payload_size);
  if (!mam_api_bundle_contains_header(bundle)) {
    return RC_MAM_BUNDLE_DOES_NOT_CONTAIN_HEADER;
  }

  mam_msg_recv_context_t ctx;
  iota_transaction_t *curr_tx = (iota_transaction_t *)utarray_eltptr(bundle, 0);
  flex_trits_to_trits(ctx.pk, NUM_TRITS_ADDRESS, transaction_address(curr_tx),
                      NUM_TRITS_ADDRESS, NUM_TRITS_ADDRESS);

  ctx.ord = 1;

  // Flatten flex_trits encoded in transaction sig_or_fragment field
  // into a single long trits_t data structure
  size_t num_trits_in_bundle =
      bundle_transactions_size(bundle) * NUM_TRITS_SIGNATURE;
  trit_t msg_trits[num_trits_in_bundle];
  trits_t msg =
      mam_api_bundle_unwrap(bundle, msg_trits, num_trits_in_bundle, 0);

  flex_trits_to_trits(msg_id, MAM_MSG_ID_SIZE, transaction_tag(curr_tx),
                      NUM_TRITS_TAG, MAM_MSG_ID_SIZE);
  ERR_BIND_RETURN(mam_msg_recv(&ctx, &msg, api->psks, api->ntru_sks,
                               trits_from_rep(MAM_MSG_ID_SIZE, msg_id)),
                  err);

  size_t packet_index = msg.d / NUM_TRITS_SIGNATURE + 1;
  if (packet_index < bundle_transactions_size(bundle)) {
    trits_advance(&msg, NUM_TRITS_SIGNATURE - msg.d % NUM_TRITS_SIGNATURE);
    ERR_BIND_RETURN(mam_api_bundle_read_packet_from_msg(api, &ctx, msg, payload,
                                                        payload_size),
                    err);
  }

  ERR_BIND_RETURN(
      trit_t_to_mam_msg_recv_context_t_map_add(&api->recv_ctxs, msg_id, ctx),
      err);

  // TODO check if last packet
  // TODO check if ord matches

  return RC_OK;
}

retcode_t mam_api_bundle_read_packet(mam_api_t *const api,
                                     bundle_transactions_t const *const bundle,
                                     tryte_t **const payload,
                                     size_t *const payload_size) {
  retcode_t ret = RC_OK;
  mam_msg_recv_context_t *ctx = NULL;
  trit_t_to_mam_msg_recv_context_t_map_entry_t *entry = NULL;
  iota_transaction_t *curr_tx = (iota_transaction_t *)utarray_eltptr(bundle, 0);
  trit_t tag[NUM_TRITS_TAG];

  flex_trits_to_trits(tag, NUM_TRITS_TAG, transaction_tag(curr_tx),
                      NUM_TRITS_TAG, NUM_TRITS_TAG);
  if (!trit_t_to_mam_msg_recv_context_t_map_find(&api->recv_ctxs, tag,
                                                 &entry) ||
      entry == NULL) {
    return RC_MAM_MESSAGE_NOT_FOUND;
  }

  size_t num_trits_in_bundle =
      bundle_transactions_size(bundle) * NUM_TRITS_SIGNATURE;
  trit_t msg_trits[num_trits_in_bundle];
  trits_t msg =
      mam_api_bundle_unwrap(bundle, msg_trits, num_trits_in_bundle, 0);

  ctx = &entry->value;

  // TODO check if last packet
  // TODO check if ord matches

  return mam_api_bundle_read_packet_from_msg(api, ctx, msg, payload,
                                             payload_size);
}

size_t mam_api_send_ctx_map_serialized_size(
    trit_t_to_mam_msg_send_context_t_map_t map) {
  trit_t_to_mam_msg_send_context_t_map_entry_t *curr_entry = NULL;
  trit_t_to_mam_msg_send_context_t_map_entry_t *tmp_entry = NULL;
  size_t serialized_size = 0;

  HASH_ITER(hh, map.map, curr_entry, tmp_entry) {
    serialized_size += (MAM_MSG_ID_SIZE +
                        mam_msg_send_ctx_serialized_size(&curr_entry->value));
  }
  return serialized_size;
}

void mam_api_send_ctx_map_serialize(trit_t_to_mam_msg_send_context_t_map_t map,
                                    trits_t *const buffer) {
  trit_t_to_mam_msg_send_context_t_map_entry_t *curr_entry = NULL;
  trit_t_to_mam_msg_send_context_t_map_entry_t *tmp_entry = NULL;

  HASH_ITER(hh, map.map, curr_entry, tmp_entry) {
    pb3_encode_ntrytes(trits_from_rep(MAM_MSG_ID_SIZE, curr_entry->key),
                       buffer);
    mam_msg_send_ctx_serialize(&curr_entry->value, buffer);
  }
}

void mam_api_recv_ctx_map_serialize(trit_t_to_mam_msg_recv_context_t_map_t map,
                                    trits_t *const buffer) {
  trit_t_to_mam_msg_recv_context_t_map_entry_t *curr_entry = NULL;
  trit_t_to_mam_msg_recv_context_t_map_entry_t *tmp_entry = NULL;

  HASH_ITER(hh, map.map, curr_entry, tmp_entry) {
    pb3_encode_ntrytes(trits_from_rep(MAM_MSG_ID_SIZE, curr_entry->key),
                       buffer);
    mam_msg_recv_ctx_serialize(&curr_entry->value, buffer);
  }
}

retcode_t mam_api_send_ctx_map_deserialize(
    trits_t const *const buffer,
    trit_t_to_mam_msg_send_context_t_map_t *const map) {
  retcode_t ret;
  mam_msg_send_context_t ctx;
  trit_t msg_id[MAM_MSG_ID_SIZE];
  const size_t num_ctxs = 0;

  pb3_decode_size_t(&num_ctxs, buffer);

  for (size_t i = 0; i < num_ctxs; ++i) {
    pb3_decode_ntrytes(trits_from_rep(MAM_MSG_ID_SIZE, msg_id), buffer);
    ERR_BIND_RETURN(mam_msg_send_ctx_deserialize(buffer, &ctx), ret);
    ERR_BIND_RETURN(trit_t_to_mam_msg_send_context_t_map_add(map, &msg_id, ctx),
                    ret);
  }

  return RC_OK;
}

retcode_t mam_api_recv_ctx_map_deserialize(
    trits_t const *const buffer,
    trit_t_to_mam_msg_recv_context_t_map_t *const map) {
  retcode_t ret;
  mam_msg_recv_context_t ctx;
  trit_t msg_id[MAM_MSG_ID_SIZE];
  const size_t num_ctxs = 0;

  pb3_decode_size_t(&num_ctxs, buffer);

  for (size_t i = 0; i < num_ctxs; ++i) {
    pb3_decode_ntrytes(trits_from_rep(MAM_MSG_ID_SIZE, msg_id), buffer);
    ERR_BIND_RETURN(mam_msg_recv_ctx_deserialize(buffer, &ctx), ret);
    ERR_BIND_RETURN(trit_t_to_mam_msg_recv_context_t_map_add(map, &msg_id, ctx),
                    ret);
  }

  return RC_OK;
}

size_t mam_api_recv_ctx_map_serialized_size(
    trit_t_to_mam_msg_recv_context_t_map_t map) {
  trit_t_to_mam_msg_recv_context_t_map_entry_t *curr_entry = NULL;
  trit_t_to_mam_msg_recv_context_t_map_entry_t *tmp_entry = NULL;
  size_t serialized_size = 0;

  HASH_ITER(hh, map.map, curr_entry, tmp_entry) {
    serialized_size += (MAM_MSG_ID_SIZE +
                        mam_msg_recv_ctx_serialized_size(&curr_entry->value));
  }
  return serialized_size;
}

size_t mam_api_serialized_size(mam_api_t const *const api) {
  return mam_prng_serialized_size() +
         pb3_sizeof_size_t(mam_ntru_sk_t_set_size(api->ntru_sks)) +
         mam_ntru_sks_serialized_size(api->ntru_sks) +
         pb3_sizeof_size_t(mam_ntru_pk_t_set_size(api->ntru_pks)) +
         mam_ntru_pks_serialized_size(api->ntru_pks) +
         pb3_sizeof_size_t(mam_psk_t_set_size(api->psks)) +
         mam_psks_serialized_size(api->psks) +
         pb3_sizeof_size_t(
             trit_t_to_mam_msg_send_context_t_map_size(&api->send_ctxs)) +
         mam_api_send_ctx_map_serialized_size(api->send_ctxs) +
         pb3_sizeof_size_t(
             trit_t_to_mam_msg_recv_context_t_map_size(&api->recv_ctxs)) +
         mam_api_recv_ctx_map_serialized_size(api->recv_ctxs) +
         mam_channels_serialized_size(api->channels);
}

void mam_api_serialize(mam_api_t const *const api, trits_t *const buffer) {
  mam_prng_serialize(&api->prng, *buffer);
  trits_advance(buffer, mam_prng_serialized_size());

  pb3_encode_size_t(mam_ntru_sk_t_set_size(api->ntru_sks), buffer);
  mam_ntru_sks_serialize(api->ntru_sks, *buffer);
  trits_advance(buffer, mam_ntru_sks_serialized_size(api->ntru_sks));
  pb3_encode_size_t(mam_ntru_pk_t_set_size(api->ntru_pks), buffer);
  mam_ntru_pks_serialize(api->ntru_pks, *buffer);
  trits_advance(buffer, mam_ntru_pks_serialized_size(api->ntru_pks));
  pb3_encode_size_t(mam_psk_t_set_size(api->psks), buffer);
  mam_psks_serialize(api->psks, *buffer);
  trits_advance(buffer, mam_psks_serialized_size(api->psks));
  pb3_encode_size_t(trit_t_to_mam_msg_send_context_t_map_size(&api->send_ctxs),
                    buffer);
  mam_api_send_ctx_map_serialize(api->send_ctxs, buffer);
  pb3_encode_size_t(trit_t_to_mam_msg_recv_context_t_map_size(&api->recv_ctxs),
                    buffer);
  mam_api_recv_ctx_map_serialize(api->recv_ctxs, buffer);
  mam_channels_serialize(api->channels, buffer);
}

retcode_t mam_api_deserialize(trits_t *const buffer, mam_api_t *const api) {
  retcode_t ret;
  size_t container_size = 0;
  trits_t current_buffer;

  api->ntru_sks = NULL;
  api->ntru_pks = NULL;
  api->psks = NULL;
  api->send_ctxs.map = NULL;
  api->recv_ctxs.map = NULL;

  trit_t_to_mam_msg_send_context_t_map_init(&api->send_ctxs, MAM_MSG_ID_SIZE);
  trit_t_to_mam_msg_recv_context_t_map_init(&api->recv_ctxs, MAM_MSG_ID_SIZE);

  mam_prng_deserialize(*buffer, &api->prng);
  trits_advance(buffer, mam_prng_serialized_size());

  pb3_decode_size_t(&container_size, buffer);
  current_buffer =
      trits_from_rep(container_size * (MAM_NTRU_PK_SIZE + MAM_NTRU_SK_SIZE),
                     buffer->p + buffer->d);
  ERR_BIND_RETURN(mam_ntru_sks_deserialize(current_buffer, &api->ntru_sks),
                  ret);
  trits_advance(buffer, container_size * (MAM_NTRU_PK_SIZE + MAM_NTRU_SK_SIZE));

  pb3_decode_size_t(&container_size, buffer);
  current_buffer = trits_from_rep(container_size * sizeof(mam_ntru_pk_t),
                                  buffer->p + buffer->d);
  ERR_BIND_RETURN(mam_ntru_pks_deserialize(current_buffer, &api->ntru_pks),
                  ret);
  trits_advance(buffer, container_size * sizeof(mam_ntru_pk_t));

  pb3_decode_size_t(&container_size, buffer);
  current_buffer =
      trits_from_rep(container_size * sizeof(mam_psk_t), buffer->p + buffer->d);
  ERR_BIND_RETURN(mam_psks_deserialize(current_buffer, &api->psks), ret);
  trits_advance(buffer, container_size * sizeof(mam_psk_t));

  ERR_BIND_RETURN(mam_api_send_ctx_map_deserialize(buffer, &api->send_ctxs),
                  ret);
  ERR_BIND_RETURN(mam_api_recv_ctx_map_deserialize(buffer, &api->recv_ctxs),
                  ret);

  ERR_BIND_RETURN(mam_channels_deserialize(buffer, &api->prng, api->channels),
                  ret);

  trit_t_to_mam_msg_send_context_t_map_entry_t *curr_ctx_entry = NULL;
  trit_t_to_mam_msg_send_context_t_map_entry_t *tmp_ctx_entry = NULL;
  mam_channel_t_set_entry_t *curr_channel_entry = NULL;
  mam_channel_t_set_entry_t *tmp_channel_entry = NULL;
  mam_endpoint_t_set_entry_t *endpoint_entry = NULL;
  mam_endpoint_t_set_entry_t *tmp_endpoint_entry = NULL;

  HASH_ITER(hh, api->send_ctxs.map, curr_ctx_entry, tmp_ctx_entry) {
    HASH_ITER(hh, api->channels, curr_channel_entry, tmp_channel_entry) {
      if (memcmp(curr_channel_entry->value.id, curr_ctx_entry->value.mss_root,
                 MAM_CHANNEL_ID_SIZE) == 0) {
        curr_ctx_entry->value.mss = &curr_channel_entry->value.mss;
      } else {
        HASH_ITER(hh, curr_channel_entry->value.endpoints, endpoint_entry,
                  tmp_endpoint_entry) {
          if (memcmp(endpoint_entry->value.id, curr_ctx_entry->value.mss_root,
                     MAM_CHANNEL_ID_SIZE) == 0) {
            curr_ctx_entry->value.mss = &endpoint_entry->value.mss;
          }
        }
      }
    }
  }

  return RC_OK;
}
