/*
 * Copyright (c) 2019 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#ifndef CCLIENT_API_IS_PROMOTABLE_H
#define CCLIENT_API_IS_PROMOTABLE_H

#include "cclient/http/http.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Checks if a transaction is _promotable_, by calling
 * iota_client_check_consistency() and verifying that
 * `attachmentTimestamp` is above a lower bound. Lower bound is calculated based
 * on number of milestones issued since transaction attachment.
 *
 * Using iota_client_is_promotable() to determine if transaction can be
 * [_promoted_] or should be [_reattached_]
 *
 * @param {iota_client_service_t} serv - client service
 * @param {trit_array_p} tail - Tail transaction hash
 * @param {boolean} out_promotable - boolean value
 *
 * @return {retcode_t}
 * - `INVALID_HASH`: Invalid hash
 * - `INVALID_DEPTH`: Invalid depth
 * - Fetch error
 *
 * https://github.com/iotaledger/iota.js/blob/next/packages/core/src/createIsPromotable.ts#L71
 */
retcode_t iota_client_is_promotable(iota_client_service_t const* const serv, flex_trit_t const* const tail_tx,
                                    bool* out_promotable);

#ifdef __cplusplus
}
#endif

#endif  // CCLIENT_API_IS_PROMOTABLE_H