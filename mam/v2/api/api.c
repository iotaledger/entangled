/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * MAM is based on an original implementation & specification by apmi.bsu.by
 * [ITSec Lab]
 *
 * Refer to the LICENSE file for licensing information
 */

#include "mam/v2/api/api.h"
#include <stdlib.h>
#include "common/model/bundle.h"
#include "mam/v2/mam/mam_psk_t_set.h"
#include "mam/v2/ntru/mam_ntru_pk_t_set.h"
#include "mam/v2/ntru/mam_ntru_sk_t_set.h"
#include "mam/v2/prng/prng.h"
#include "utils/time.h"

retcode_t mam_api_init(mam_api_t *const api, tryte_t const *const mam_seed) {
  retcode_t ret = RC_OK;

  if (api == NULL || mam_seed == NULL) {
    return RC_NULL_PARAM;
  }

  MAM2_TRITS_DEF0(mam_seed_trits, MAM2_PRNG_KEY_SIZE);
  mam_seed_trits = MAM2_TRITS_INIT(mam_seed_trits, MAM2_PRNG_KEY_SIZE);
  trits_from_str(mam_seed_trits, (char const *)mam_seed);

  if ((ret = mam_prng_init(&api->prng, mam_seed_trits)) != RC_OK) {
    return ret;
  }
  trits_set_zero(mam_seed_trits);
  api->ntru_sks = NULL;
  api->ntru_pks = NULL;
  api->psks = NULL;
  if ((ret = trit_t_to_mam_msg_send_context_t_map_init(
           &api->send_ctxs, MAM2_MSG_ID_SIZE)) != RC_OK) {
    return ret;
  }

  return ret;
}

retcode_t mam_api_destroy(mam_api_t *const api) {
  retcode_t ret = RC_OK;

  if (api == NULL) {
    return RC_NULL_PARAM;
  }

  ret = mam_prng_destroy(&api->prng);
  mam_ntru_sk_t_set_free(&api->ntru_sks);
  mam_ntru_pk_t_set_free(&api->ntru_pks);
  mam_psk_t_set_free(&api->psks);
  ret = trit_t_to_mam_msg_send_context_t_map_free(&api->send_ctxs);

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
    trits_t header_part = trits_null();
    iota_transaction_t transaction;
    flex_trit_t buffer[FLEX_TRIT_SIZE_6561];

    header_size = mam_msg_send_size(ch, ep, ch1, ep1, psks, ntru_pks);
    if (trits_is_null(header = trits_alloc(header_size))) {
      return RC_OOM;
    }

    mam_msg_send(&ctx, &api->prng, ch, ep, ch1, ep1,
                 trits_from_rep(MAM2_MSG_ID_SIZE, msg_id), msg_type_id, psks,
                 ntru_pks, &header);
    header = trits_pickup(header, header_size);

    transaction_reset(&transaction);
    flex_trits_from_trits(buffer, NUM_TRITS_ADDRESS, ch->id, NUM_TRITS_ADDRESS,
                          NUM_TRITS_ADDRESS);
    transaction_set_address(&transaction, buffer);
    transaction_set_value(&transaction, 0);
    transaction_set_obsolete_tag(&transaction,
                                 transaction.data.signature_or_message);
    transaction_set_timestamp(&transaction, current_timestamp_ms() / 1000);
    transaction_set_last_index(&transaction,
                               (header_size - 1) / NUM_TRITS_SIGNATURE);
    transaction_set_tag(&transaction, transaction.data.signature_or_message);

    for (size_t current_index = 0; !trits_is_empty(header); current_index++) {
      header_part = trits_take_min(header, NUM_TRITS_SIGNATURE);
      header = trits_drop_min(header, NUM_TRITS_SIGNATURE);
      transaction_set_current_index(&transaction, current_index);
      memset(buffer, FLEX_TRIT_NULL_VALUE, FLEX_TRIT_SIZE_6561);
      flex_trits_from_trits(buffer, NUM_TRITS_SIGNATURE,
                            header_part.p + header_part.d,
                            trits_size(header_part), trits_size(header_part));
      transaction_set_message(&transaction, buffer);
      bundle_transactions_add(bundle, &transaction);
    }

    trits_free(header);
  }

  ctx.ord = 0;
  ctx.mss = NULL;

  return trit_t_to_mam_msg_send_context_t_map_add(&api->send_ctxs, msg_id, ctx);
}

retcode_t mam_api_bundle_write_packet(mam_api_t *const api,
                                      trit_t *const msg_id,
                                      tryte_t const *const payload,
                                      mam_msg_checksum_t checksum,
                                      bundle_transactions_t *const bundle) {
  mam_msg_send_context_t *ctx = NULL;
  trit_t_to_mam_msg_send_context_t_map_entry_t *entry = NULL;

  if (api == NULL || payload == NULL || bundle == NULL) {
    return RC_NULL_PARAM;
  }

  bool found = trit_t_to_mam_msg_send_context_t_map_find(&api->send_ctxs,
                                                         msg_id, &entry);

  // TODO check if bundle contains header

  // TODO fetch pending state from msgID
  // TODO copy or get spongos state

  // TODO increment ord

  // TODO if last remove pending state

  return RC_OK;
}

bool mam_api_bundle_contains_header(bundle_transactions_t const *const bundle) {
  iota_transaction_t *tx = (iota_transaction_t *)utarray_front(bundle);
  return flex_trits_are_null(transaction_tag(tx), NUM_TRITS_TAG);
}

retcode_t mam_api_bundle_read_msg(mam_recv_msg_context_t *const ctx,
                                  mam_channel_t const *const cha,
                                  bundle_transactions_t const *const bundle,
                                  flex_trit_t **const packet_payload,
                                  trits_t session_key, trits_t msg_id) {
  retcode_t err = RC_OK;
  MAM2_ASSERT(packet_payload && *packet_payload == NULL);
  if (!mam_api_bundle_contains_header(bundle)) {
    return RC_MAM2_BUNDLE_DOES_NOT_CONTAIN_HEADER;
  }

  // Flatten flex_trits encoded in transaction sig_or_fragment field
  // into a single long trits_t data structure
  size_t num_trits_in_bundle =
      bundle_transactions_size(bundle) * NUM_TRITS_SIGNATURE;
  trit_t msg_trits[num_trits_in_bundle];
  iota_transaction_t *curr_tx = (iota_transaction_t *)utarray_eltptr(bundle, 0);

  trit_t *msg_trits_ptr = msg_trits;
  BUNDLE_FOREACH(bundle, curr_tx) {
    flex_trits_to_trits(msg_trits_ptr, NUM_TRITS_SIGNATURE,
                        transaction_signature(curr_tx), NUM_TRITS_SIGNATURE,
                        NUM_TRITS_SIGNATURE);
    msg_trits_ptr += NUM_TRITS_SIGNATURE;
  }

  trits_t msg = trits_from_rep(num_trits_in_bundle, msg_trits);
  ERR_BIND_RETURN(mam_recv_msg(ctx, &msg, session_key), err);

  size_t packet_index = msg.d / NUM_TRITS_SIGNATURE + 1;
  if (packet_index < bundle_transactions_size(bundle) - 1) {
    size_t num_trits_in_packet =
        (bundle_transactions_size(bundle) - 1 - packet_index) *
        NUM_TRITS_SIGNATURE;
    trit_t packet_trits[num_trits_in_packet];
    trits_t packet = trits_from_rep(num_trits_in_packet, packet_trits);
    ERR_BIND_RETURN(mam_recv_packet(ctx, &msg, &packet), err);

    *packet_payload = malloc(NUM_FLEX_TRITS_FOR_TRITS(num_trits_in_packet));
    flex_trits_from_trits(*packet_payload, num_trits_in_packet, packet_trits,
                          num_trits_in_packet, num_trits_in_packet);
  }

  return RC_OK;
}

retcode_t mam_api_bundle_read_packet(mam_recv_msg_context_t *const ctx,
                                     mam_channel_t const *const cha,
                                     bundle_transactions_t const *const bundle,
                                     trits_t const session_key,
                                     flex_trit_t *const payload, uint32_t ord,
                                     trits_t const msg_id) {}