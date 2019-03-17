/*
 * Copyright (c) 2019 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#include "cclient/http/http.h"
#include "common/model/bundle.h"

retcode_t traverse_bundle(iota_client_service_t const* const serv, flex_trit_t const* const tail_hash,
                          bundle_transactions_t* const bundle, hash8019_array_p trytes);

/**
 * Fetches the bundle of a given the _tail_ transaction hash, by traversing
 * through `trunkTransaction`. It does not validate the bundle.
 *
 * @param {flex_trit_t} tail_hash- Trunk transaction, should be tail
 * (`currentIndex == 0`)
 * @param {hashes_t} bundles - List of accumulated transactions
 * @param {bundle_transactions_t} bundle - Bundle as array of transaction
 * objects
 *
 * @returns {retcode_t}
 * - `INVALID_TRANSACTION_HASH`
 * - `INVALID_TAIL_HASH`: Provided transaction is not tail (`currentIndex !==
 * 0`)
 * - `INVALID_BUNDLE`: Bundle is syntactically invalid
 * - Fetch error
 *
 * https://github.com/iotaledger/iota.js/blob/next/packages/core/src/createTraverseBundle.ts#L36
 */
retcode_t iota_client_traverse_bundle(iota_client_service_t const* const serv, flex_trit_t const* const tail_hash,
                                      bundle_transactions_t* const bundle);
