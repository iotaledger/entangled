/*
 * Copyright (c) 2019 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#include "ciri/consensus/test_utils/bundle.h"
#include <stdlib.h>
#include "common/helpers/digest.h"
#include "common/helpers/pow.h"
#include "common/helpers/sign.h"
#include "utils/time.h"

retcode_t iota_test_utils_bundle_create_transfer(flex_trit_t const *const branch, flex_trit_t const *const trunk,
                                                 bundle_transactions_t *bundle, flex_trit_t const *const seed,
                                                 uint64_t sk_index, int64_t value) {
  retcode_t ret;
  iota_transaction_t tx;
  iota_transaction_t *tx_iter;
  flex_trit_t *sig = NULL;
  flex_trit_t *inp_address = NULL;
  flex_trit_t *out_address = NULL;
  flex_trit_t txflex[FLEX_TRIT_SIZE_8019];
  flex_trit_t bundle_hash[FLEX_TRIT_SIZE_243];
  flex_trit_t *curr_hash;
  Kerl kerl;
  kerl_init(&kerl);

  transaction_reset(&tx);

  inp_address = iota_sign_address_gen_flex_trits(seed, sk_index, 1);
  out_address = iota_sign_address_gen_flex_trits(seed, sk_index + 1, 1);
  transaction_set_value(&tx, -value);
  transaction_set_current_index(&tx, 0);
  transaction_set_address(&tx, inp_address);
  transaction_set_last_index(&tx, 1);
  transaction_set_timestamp(&tx, current_timestamp_ms());
  tx.loaded_columns_mask.attachment |= MASK_ATTACHMENT_TAG;
  tx.loaded_columns_mask.essence |= MASK_ESSENCE_OBSOLETE_TAG;

  bundle_transactions_add(bundle, &tx);

  transaction_set_value(&tx, value);
  transaction_set_current_index(&tx, 1);
  transaction_set_address(&tx, out_address);
  transaction_set_last_index(&tx, 1);

  bundle_transactions_add(bundle, &tx);

  bundle_calculate_hash(bundle, &kerl, bundle_hash);
  transaction_set_bundle(bundle_at(bundle, 0), bundle_hash);
  transaction_set_bundle(bundle_at(bundle, 1), bundle_hash);

  if (value > 0) {
    sig = iota_sign_signature_gen_flex_trits(seed, sk_index, 1, bundle_hash);
    transaction_set_signature(bundle_at(bundle, 0), sig);
  }

  ERR_BIND_GOTO(iota_pow_bundle(bundle, trunk, branch, 9), ret, cleanup);

  BUNDLE_FOREACH(bundle, tx_iter) {
    transaction_serialize_on_flex_trits(tx_iter, txflex);
    curr_hash = iota_flex_digest(txflex, NUM_TRITS_SERIALIZED_TRANSACTION);
    transaction_set_hash(tx_iter, curr_hash);
    free(curr_hash);
  }

cleanup:
  if (sig) {
    free(sig);
  }
  if (inp_address) {
    free(inp_address);
  }

  if (out_address) {
    free(out_address);
  }

  return ret;
}
