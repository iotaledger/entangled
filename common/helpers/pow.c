/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#include <stdlib.h>
#include <string.h>

#include "utarray.h"

#include "common/crypto/curl-p/hashcash.h"
#include "common/helpers/digest.h"
#include "common/helpers/pow.h"
#include "common/trinary/trit_tryte.h"
#include "utils/export.h"
#include "utils/time.h"

#define NONCE_LENGTH 27 * 3

trit_t *do_pow(Curl *const curl, trit_t const *const trits_in, size_t const trits_len, uint8_t const mwm) {
  tryte_t *nonce_trits = (tryte_t *)calloc(NONCE_LENGTH + 1, sizeof(tryte_t));

  curl_absorb(curl, trits_in, trits_len - HASH_LENGTH_TRIT);
  memcpy(curl->state, trits_in + trits_len - HASH_LENGTH_TRIT, HASH_LENGTH_TRIT);

  // FIXME(th0br0) deal with result value of `hashcash` call
  hashcash(curl, BODY, HASH_LENGTH_TRIT - NONCE_LENGTH, HASH_LENGTH_TRIT, mwm);

  memcpy(nonce_trits, curl->state + HASH_LENGTH_TRIT - NONCE_LENGTH, NONCE_LENGTH);

  return nonce_trits;
}

IOTA_EXPORT char *iota_pow_trytes(char const *const trytes_in, uint8_t const mwm) {
  Curl curl;
  curl_init(&curl);
  curl.type = CURL_P_81;

  int tryte_len = strlen(trytes_in);
  int trits_len = tryte_len * 3;

  trit_t *trits = (trit_t *)calloc(trits_len, sizeof(trit_t));
  if (!trits) {
    return NULL;
  }
  trytes_to_trits((tryte_t *)trytes_in, trits, tryte_len);

  trit_t *nonce_trits = do_pow(&curl, trits, trits_len, mwm);
  free(trits);
  if (!nonce_trits) {
    return NULL;
  }
  tryte_t *nonce_trytes = (tryte_t *)calloc(NUM_TRITS_NONCE / 3 + 1, sizeof(tryte_t));
  if (!nonce_trytes) {
    free(nonce_trits);
    return NULL;
  }
  trits_to_trytes(nonce_trits, nonce_trytes, NUM_TRITS_NONCE);
  free(nonce_trits);

  return (char *)nonce_trytes;
}

IOTA_EXPORT flex_trit_t *iota_pow_flex(flex_trit_t const *const flex_trits_in, size_t num_trits, uint8_t const mwm) {
  Curl curl;
  curl_init(&curl);
  curl.type = CURL_P_81;

  trit_t *trits = (trit_t *)calloc(num_trits, sizeof(trit_t));
  if (!trits) {
    return NULL;
  }
  flex_trits_to_trits(trits, num_trits, flex_trits_in, num_trits, num_trits);
  trit_t *nonce_trits = do_pow(&curl, trits, num_trits, mwm);
  free(trits);
  if (!nonce_trits) {
    return NULL;
  }
  flex_trit_t *nonce_flex_trits = (flex_trit_t *)calloc(NUM_TRITS_NONCE, sizeof(flex_trit_t));
  if (!nonce_flex_trits) {
    free(nonce_trits);
    return NULL;
  }
  flex_trits_from_trits(nonce_flex_trits, NUM_TRITS_NONCE, nonce_trits, NUM_TRITS_NONCE, NUM_TRITS_NONCE);
  free(nonce_trits);

  return nonce_flex_trits;
}

IOTA_EXPORT retcode_t iota_pow_bundle(bundle_transactions_t *const bundle, flex_trit_t const *const trunk,
                                      flex_trit_t const *const branch, uint8_t const mwm) {
  flex_trit_t txflex[FLEX_TRIT_SIZE_8019];
  iota_transaction_t *tx = NULL;
  flex_trit_t *nonce = NULL;
  flex_trit_t *ctrunk = NULL;
  size_t cur_idx = 0;

  if (bundle == NULL || trunk == NULL || branch == NULL) {
    return RC_NULL_PARAM;
  }

  if (bundle_transactions_size(bundle) == 0) {
    return RC_OK;
  }

  tx = (iota_transaction_t *)utarray_front(bundle);
  cur_idx = tx->essence.last_index + 1;

  ctrunk = (flex_trit_t *)trunk;

  do {
    cur_idx--;

    // Find current tx
    for (tx = (iota_transaction_t *)utarray_front(bundle); tx != NULL && tx->essence.current_index != cur_idx;
         tx = (iota_transaction_t *)utarray_next(bundle, tx))
      ;

    if (tx == NULL) {
      return RC_HELPERS_POW_INVALID_TX;
    }

    // Set trunk & branch
    transaction_set_trunk(tx, ctrunk);
    transaction_set_branch(tx, branch);
    transaction_set_attachment_timestamp(tx, current_timestamp_ms());
    transaction_set_attachment_timestamp_lower(tx, 0);
    transaction_set_attachment_timestamp_upper(tx, 3812798742493LL);

    transaction_serialize_on_flex_trits(tx, txflex);

    // Do PoW
    if ((nonce = iota_pow_flex(txflex, NUM_TRITS_SERIALIZED_TRANSACTION, mwm)) == NULL) {
      return RC_OOM;
    }
    transaction_set_nonce(tx, nonce);
    free(nonce);

    if (ctrunk != trunk) {
      free(ctrunk);
    }

    transaction_serialize_on_flex_trits(tx, txflex);

    if ((ctrunk = iota_flex_digest(txflex, NUM_TRITS_SERIALIZED_TRANSACTION)) == NULL) {
      return RC_OOM;
    }

  } while (cur_idx != 0);

  if (ctrunk != trunk) {
    free(ctrunk);
  }

  return RC_OK;
}
