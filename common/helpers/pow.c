/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#include "common/helpers/pow.h"

#include <stdlib.h>
#include <string.h>

#include "common/curl-p/hashcash.h"
#include "common/errors.h"
#include "common/model/bundle.h"
#include "common/pow/pow.h"
#include "common/trinary/trit_tryte.h"
#include "digest.h"
#include "utarray.h"
#include "utils/export.h"

IOTA_EXPORT char *iota_pow_trytes(char const *const trytes_in,
                                  uint8_t const mwm) {
  Curl curl;
  init_curl(&curl);
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
  tryte_t *nonce_trytes =
      (tryte_t *)calloc(NUM_TRITS_NONCE / 3 + 1, sizeof(tryte_t));
  if (!nonce_trytes) {
    free(nonce_trits);
    return NULL;
  }
  trits_to_trytes(nonce_trits, nonce_trytes, NUM_TRITS_NONCE);
  free(nonce_trits);

  return (char *)nonce_trytes;
}

IOTA_EXPORT flex_trit_t *iota_pow_flex(flex_trit_t const *const flex_trits_in,
                                       size_t num_trits, uint8_t const mwm) {
  Curl curl;
  init_curl(&curl);
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
  size_t flex_len = NUM_FLEX_TRITS_FOR_TRITS(NUM_TRITS_NONCE);
  flex_trit_t *nonce_flex_trits =
      (flex_trit_t *)calloc(flex_len, sizeof(flex_trit_t));
  if (!nonce_flex_trits) {
    free(nonce_trits);
    return NULL;
  }
  flex_trits_from_trits(nonce_flex_trits, NUM_TRITS_NONCE, nonce_trits,
                        NUM_TRITS_NONCE, NUM_TRITS_NONCE);
  free(nonce_trits);

  return nonce_flex_trits;
}

IOTA_EXPORT retcode_t iota_pow_bundle(bundle_transactions_t *const bundle,
                                      flex_trit_t const *const trunk,
                                      flex_trit_t const *const branch,
                                      uint8_t const mwm) {
  iota_transaction_t *tx;
  flex_trit_t *nonce, *txflex, *ctrunk;
  size_t cur_idx = 0;

  tx = utarray_front(bundle);
  cur_idx = tx->essence.last_index + 1;

  ctrunk = trunk;

  do {
    cur_idx--;

    // Find current tx
    for (tx = (iota_transaction_t *)utarray_front(bundle);
         tx != NULL && tx->essence.current_index != cur_idx;
         tx = (iota_transaction_t *)utarray_next(bundle, tx))
      ;

    // Set trunk & branch
    transaction_set_trunk(tx, ctrunk);
    transaction_set_branch(tx, branch);

    txflex = transaction_serialize(tx);

    if (txflex == NULL) {
      return RC_OOM;
    }

    // Do PoW
    nonce = iota_pow_flex(txflex, NUM_TRITS_SERIALIZED_TRANSACTION, mwm);
    if (nonce == NULL) {
      return RC_OOM;
    }
    transaction_set_nonce(tx, nonce);
    free(nonce);

    if (ctrunk != trunk) {
      free(ctrunk);
    }

    ctrunk = iota_flex_digest(txflex, NUM_TRITS_SERIALIZED_TRANSACTION);
    free(txflex);
  } while (cur_idx != 0);

  return RC_OK;
}