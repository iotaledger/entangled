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
#ifndef CCLIENT_API_GET_NEW_ADDRESS_H
#define CCLIENT_API_GET_NEW_ADDRESS_H

#include "cclient/api/extended/types.h"
#include "cclient/http/http.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Generates and retruns addresses including an unused address.
 *
 * Generates and retruns addresses by calling #iota_client_find_transactions until the first unused address is detected.
 *
 * @param[in] serv client service
 * @param[in] seed A seed for address generation.
 * @param[in] addr_opt address information containing security level and key indices
 * @param[out] out_addresses List of addresses including an unused address.
 * @return #retcode_t
 */
retcode_t iota_client_get_new_address(iota_client_service_t const* const serv, flex_trit_t const* const seed,
                                      address_opt_t const addr_opt, hash243_queue_t* out_addresses);

#ifdef __cplusplus
}
#endif

#endif  // CCLIENT_API_GET_NEW_ADDRESS_H

/** @} */