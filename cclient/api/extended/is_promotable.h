/*
 * Copyright (c) 2019 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

/**
 * @ingroup cclient_extended
 *
 * @{
 *
 * @file
 * @brief
 *
 */
#ifndef CCLIENT_API_IS_PROMOTABLE_H
#define CCLIENT_API_IS_PROMOTABLE_H

#include "cclient/http/http.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Checks if a tail transaction is <b>promotable</b>
 *
 * Checks if a transaction is <b>promotable</b>, by calling #iota_client_check_consistency and verifying that
 * <b>attachment_timestamp</b> is above a lower bound in the transaction. Lower bound is calculated based on number of
 * milestones issued since transaction attachment.
 *
 * @param[in] serv client service
 * @param[in] tail_tx Tail transaction hash
 * @param[out] out_promotable <b>True</b> if it is, otherwise <b>False</b>
 * @return #retcode_t
 */
retcode_t iota_client_is_promotable(iota_client_service_t const* const serv, flex_trit_t const* const tail_tx,
                                    bool* out_promotable);

#ifdef __cplusplus
}
#endif

#endif  // CCLIENT_API_IS_PROMOTABLE_H

/** @} */