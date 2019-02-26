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
#include "mam/v2/api/api.h"
#include "mam/v2/ntru/mam_ntru_pk_t_set.h"
#include "mam/v2/ntru/mam_ntru_sk_t_set.h"
#include "mam/v2/prng/prng.h"
#include "mam/v2/psk/mam_psk_t_set.h"
#include "utils/time.h"

/*
 * Private functions
 */

static void mam_api_bundle_wrap(bundle_transactions_t *const bundle,
                                trit_t *address, trits_t message) {
  iota_transaction_t transaction;
  flex_trit_t buffer[FLEX_TRIT_SIZE_6561];
  trits_t message_part = trits_null();

  transaction_reset(&transaction);
  flex_trits_from_trits(buffer, NUM_TRITS_ADDRESS, address, NUM_TRITS_ADDRESS,
                        NUM_TRITS_ADDRESS);
  transaction_set_address(&transaction, buffer);
  transaction_set_value(&transaction, 0);
  transaction_set_obsolete_tag(&transaction,
                               transaction.data.signature_or_message);
  transaction_set_timestamp(&transaction, current_timestamp_ms() / 1000);
  transaction_set_tag(&transaction, transaction.data.signature_or_message);

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

/*
 * Public functions
 */

retcode_t mam_api_init(mam_api_t *const api, tryte_t const *const mam_seed) {
  retcode_t ret = RC_OK;

  if (api == NULL || mam_seed == NULL) {
    return RC_NULL_PARAM;
  }

  MAM2_TRITS_DEF0(mam_seed_trits, MAM2_PRNG_KEY_SIZE);
  mam_seed_trits = MAM2_TRITS_INIT(mam_seed_trits, MAM2_PRNG_KEY_SIZE);
  trits_from_str(mam_seed_trits, (char const *)mam_seed);

  mam_prng_init(&api->prng, mam_seed_trits);
  trits_set_zero(mam_seed_trits);
  api->ntru_sks = NULL;
  api->ntru_pks = NULL;
  api->psks = NULL;

  ERR_BIND_RETURN(trit_t_to_mam_msg_send_context_t_map_init(&api->send_ctxs,
                                                            MAM2_MSG_ID_SIZE),
                  ret);
  ERR_BIND_RETURN(trit_t_to_mam_msg_recv_context_t_map_init(&api->recv_ctxs,
                                                            MAM2_MSG_ID_SIZE),
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

retcode_t mam_api_bundle_write_header(
    mam_api_t *const api, mam_channel_t *const ch,
    mam_endpoint_t const *const ep, mam_channel_t const *const ch1,
    mam_endpoint_t const *const ep1, mam_psk_t_set_t psks,
    mam_ntru_pk_t_set_t ntru_pks, trint9_t msg_type_id,
    bundle_transactions_t *const bundle, trit_t *const msg_id) {
  retcode_t ret = RC_OK;
  mam_msg_send_context_t ctx;

  if (api == NULL || ch == NULL || bundle == NULL || msg_id == NULL) {
    return RC_NULL_PARAM;
  }

  if (bundle_transactions_size(bundle) != 0) {
    return RC_MAM2_BUNDLE_NOT_EMPTY;
  }

  // TODO add a random part
  {
    trits_t msg_id_parts[] = {mam_channel_name(ch), mam_channel_msg_ord(ch)};

    mam_spongos_hashn(&ctx.spongos, 2, msg_id_parts,
                      trits_from_rep(MAM2_MSG_ID_SIZE, msg_id));
    add_assign(ch->msg_ord, MAM2_MSG_ID_SIZE, 1);
  }

  {
    trits_t header = trits_null();
    size_t header_size = 0;

    header_size = mam_msg_send_size(ch, ep, ch1, ep1, psks, ntru_pks);
    if (trits_is_null(header = trits_alloc(header_size))) {
      return RC_OOM;
    }
    mam_msg_send(&ctx, &api->prng, ch, ep, ch1, ep1,
                 trits_from_rep(MAM2_MSG_ID_SIZE, msg_id), msg_type_id, psks,
                 ntru_pks, &header);
    header = trits_pickup(header, header_size);
    mam_api_bundle_wrap(bundle, ch->id, header);
    trits_free(header);
  }

  ctx.ord = 0;
  // TODO set the right MSS
  ctx.mss = NULL;

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
    return RC_MAM2_MESSAGE_NOT_FOUND;
  }
  ctx = &entry->value;

  {
    trits_t packet = trits_null();
    size_t packet_size = 0;
    MAM2_TRITS_DEF0(payload_trits, payload_size * 3);
    payload_trits = MAM2_TRITS_INIT(payload_trits, payload_size * 3);
    trits_from_str(payload_trits, payload);

    packet_size =
        mam_msg_send_packet_size(checksum, ctx->mss, payload_size * 3);
    if (trits_is_null(packet = trits_alloc(packet_size))) {
      return RC_OOM;
    }

    mam_msg_send_packet(ctx, checksum, payload_trits, &packet);
    packet = trits_pickup(packet, packet_size);
    mam_api_bundle_wrap(bundle, ch->id, packet);
    trits_free(packet);
  }

  ctx->ord++;

  // TODO check if bundle contains header
  // TODO if last remove pending state

  return RC_OK;
}

bool mam_api_bundle_contains_header(bundle_transactions_t const *const bundle) {
  iota_transaction_t *tx = (iota_transaction_t *)utarray_front(bundle);
  return flex_trits_are_null(transaction_tag(tx), FLEX_TRIT_SIZE_81);
}

trits_t msg_trits_from_bundle(bundle_transactions_t const *const bundle,
                              const trit_t msg_trits[],
                              size_t num_trits_in_bundle, size_t start_index) {
  iota_transaction_t *curr_tx = (iota_transaction_t *)utarray_eltptr(bundle, 0);

  trit_t *msg_trits_ptr = msg_trits;
  size_t curr_tx_index = 0;
  BUNDLE_FOREACH(bundle, curr_tx) {
    if (curr_tx_index++ < start_index) {
      continue;
    }
    flex_trits_to_trits(msg_trits_ptr, NUM_TRITS_SIGNATURE,
                        transaction_signature(curr_tx), NUM_TRITS_SIGNATURE,
                        NUM_TRITS_SIGNATURE);
    msg_trits_ptr += NUM_TRITS_SIGNATURE;
  }

  return trits_from_rep(num_trits_in_bundle, msg_trits);
}

static retcode_t mam_api_bundle_read_header(
    mam_msg_recv_context_t const *const ctx, trits_t *const msg,
    mam_psk_t_set_t psks, mam_ntru_sk_t_set_t ntru_sks, trits_t msg_id) {
  retcode_t err;
  ERR_BIND_RETURN(mam_msg_recv(ctx, msg, psks, ntru_sks, msg_id), err);
  return err;
}

retcode_t mam_api_bundle_read_msg(mam_api_t *const api,
                                  bundle_transactions_t const *const bundle,
                                  tryte_t **const payload,
                                  size_t *const payload_size) {
  retcode_t err = RC_OK;
  trit_t msg_id[MAM2_MSG_ID_SIZE];

  MAM2_ASSERT(payload && *payload == NULL && payload_size);
  if (!mam_api_bundle_contains_header(bundle)) {
    return RC_MAM2_BUNDLE_DOES_NOT_CONTAIN_HEADER;
  }

  mam_msg_recv_context_t ctx;
  iota_transaction_t *curr_tx = (iota_transaction_t *)utarray_eltptr(bundle, 0);
  flex_trits_to_trits(ctx.pk, NUM_TRITS_ADDRESS, transaction_address(curr_tx),
                      NUM_TRITS_ADDRESS, NUM_TRITS_ADDRESS);

  // Flatten flex_trits encoded in transaction sig_or_fragment field
  // into a single long trits_t data structure
  size_t num_trits_in_bundle =
      bundle_transactions_size(bundle) * NUM_TRITS_SIGNATURE;
  trit_t msg_trits[num_trits_in_bundle];
  trits_t msg =
      msg_trits_from_bundle(bundle, msg_trits, num_trits_in_bundle, 0);

  ERR_BIND_RETURN(
      mam_api_bundle_read_header(&ctx, &msg, api->psks, api->ntru_sks,
                                 trits_from_rep(MAM2_MSG_ID_SIZE, msg_id)),
      err);

  ERR_BIND_RETURN(
      trit_t_to_mam_msg_recv_context_t_map_add(&api->recv_ctxs, msg_id, ctx),
      err);

  size_t packet_index = msg.d / NUM_TRITS_SIGNATURE + 1;
  if (packet_index < bundle_transactions_size(bundle)) {
    size_t num_trits_in_packet =
        (bundle_transactions_size(bundle) - packet_index) * NUM_TRITS_SIGNATURE;
    trits_t packet = trits_null();
    msg = trits_drop(msg, NUM_TRITS_SIGNATURE - msg.d);
    ERR_BIND_RETURN(mam_msg_recv_packet(&ctx, &msg, &packet), err);

    *payload_size = trits_size(packet) / 3;
    *payload = malloc(*payload_size * sizeof(tryte_t));
    trits_to_trytes(packet.p, *payload, *payload_size * 3);
  }

  return RC_OK;
}

retcode_t mam_api_bundle_read_packet(mam_api_t const *const api,
                                     bundle_transactions_t const *const bundle,
                                     flex_trit_t **const packet_payload) {
  retcode_t err;
  trit_t_to_mam_msg_recv_context_t_map_entry_t *entry = NULL;

  iota_transaction_t *curr_tx = (iota_transaction_t *)utarray_eltptr(bundle, 0);
  trit_t msg_id[NUM_TRITS_TAG];
  flex_trits_to_trits(msg_id, NUM_TRITS_TAG, transaction_tag(curr_tx),
                      NUM_TRITS_TAG, NUM_TRITS_TAG);
  bool found = trit_t_to_mam_msg_recv_context_t_map_find(&api->recv_ctxs,
                                                         msg_id, &entry);
  size_t num_trits_in_bundle =
      bundle_transactions_size(bundle) * NUM_TRITS_SIGNATURE;
  trit_t msg_trits[num_trits_in_bundle];
  trits_t msg =
      msg_trits_from_bundle(bundle, msg_trits, num_trits_in_bundle, 0);

  if (found) {
    size_t num_trits_in_packet =
        bundle_transactions_size(bundle) * NUM_TRITS_SIGNATURE;
    trit_t packet_trits[num_trits_in_packet];
    trits_t packet = trits_from_rep(num_trits_in_packet, packet_trits);
    ERR_BIND_RETURN(mam_msg_recv_packet(&entry->value, &msg, &packet), err);
    *packet_payload = malloc(NUM_FLEX_TRITS_FOR_TRITS(num_trits_in_packet));
    flex_trits_from_trits(*packet_payload, num_trits_in_packet, packet_trits,
                          num_trits_in_packet, num_trits_in_packet);
  } else {
    return RC_MAM2_RECV_CTX_NOT_FOUND;
  }
  return RC_OK;
}
