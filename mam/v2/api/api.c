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

  return ret;
}

retcode_t mam_api_destroy(mam_api_t *const api) {
  retcode_t ret = RC_OK;

  if (api == NULL) {
    return RC_NULL_PARAM;
  }

  if ((ret = mam_prng_destroy(&api->prng)) != RC_OK) {
    return ret;
  }
  mam_ntru_sk_t_set_free(&api->ntru_sks);
  mam_ntru_pk_t_set_free(&api->ntru_pks);
  mam_psk_t_set_free(&api->psks);

  return RC_OK;
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
    mam_api_t *const api, mam_channel_t const *const ch,
    mam_endpoint_t const *const ep, mam_channel_t const *const ch1,
    mam_endpoint_t const *const ep1, mam_psk_t_set_t psks,
    mam_ntru_pk_t_set_t ntru_pks, trint9_t msg_type_id,
    bundle_transactions_t *const bundle) {
  trits_t header = trits_null();
  size_t header_size = 0;
  trits_t header_part = trits_null();
  mam_send_context_t ctx;
  iota_transaction_t transaction;
  size_t current_index = 0;
  MAM2_TRITS_DEF0(msg_id, MAM2_HEADER_MSG_ID_SIZE);
  msg_id = MAM2_TRITS_INIT(msg_id, MAM2_HEADER_MSG_ID_SIZE);

  if (api == NULL || ch == NULL || bundle == NULL) {
    return RC_NULL_PARAM;
  }

  if (bundle_transactions_size(bundle) != 0) {
    return RC_MAM2_BUNDLE_NOT_EMPTY;
  }

  // TODO add a random part
  trits_t msg_id_parts[] = {mam_channel_name(ch), mam_channel_msg_ord(ch)};
  mam_spongos_hashn(&ctx.spongos, 2, msg_id_parts, msg_id);
  add_assign(ch->msg_ord, MAM2_HEADER_MSG_ID_SIZE, 1);

  header_size = mam_send_msg_size(ch, ep, ch1, ep1, psks, ntru_pks);
  if (trits_is_null(header = trits_alloc(header_size))) {
    return RC_OOM;
  }

  mam_send_msg(&ctx, &api->prng, ch, ep, ch1, ep1, msg_id, msg_type_id, psks,
               ntru_pks, &header);
  header = trits_pickup(header, header_size);

  transaction_reset(&transaction);
  // TODO setter ?
  flex_trits_from_trits(transaction.essence.address, NUM_TRITS_ADDRESS, ch->id,
                        NUM_TRITS_ADDRESS, NUM_TRITS_ADDRESS);
  transaction_set_last_index(&transaction,
                             (header_size - 1) / NUM_TRITS_SIGNATURE);
  // TODO set masks ?
  transaction.loaded_columns_mask.essence = MASK_ESSENCE_ALL;
  while (!trits_is_empty(header)) {
    header_part = trits_take_min(header, NUM_TRITS_SIGNATURE);
    header = trits_drop_min(header, NUM_TRITS_SIGNATURE);
    transaction_set_timestamp(&transaction, current_timestamp_ms() / 1000);
    transaction_set_current_index(&transaction, current_index);
    // TODO setter ?
    flex_trits_from_trits(transaction.data.signature_or_message,
                          NUM_TRITS_SIGNATURE, header_part.p + header_part.d,
                          trits_size(header_part), trits_size(header_part));
    bundle_transactions_add(bundle, &transaction);
    current_index++;
  }

  // TODO Add to pending states
  // Init ord to 0
  // copy spongos there

  trits_free(header);

  return RC_OK;
}

retcode_t mam_api_bundle_write_packet(mam_api_t *const api,
                                      tryte_t const *const payload,
                                      mam_msg_checksum_t checksum,
                                      bundle_transactions_t *const bundle) {
  mam_send_context_t *ctx = NULL;

  if (api == NULL || payload == NULL || bundle == NULL) {
    return RC_NULL_PARAM;
  }

  // TODO check if bundle contains header

  // TODO fetch pending state from msgID
  // TODO copy or get spongos state

  // TODO increment ord

  // TODO if last remove pending state

  return RC_OK;
}

retcode_t mam_api_bundle_read(mam_api_t *const api,
                              mam_channel_t const *const cha,
                              bundle_transactions_t const *const bundle,
                              flex_trit_t *const payload) {
  if (api == NULL || cha == NULL || bundle == NULL || payload == NULL) {
    return RC_NULL_PARAM;
  }
  return RC_OK;
}
