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

#include "cclient/http/http.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Options for address generation
 *
 */
typedef struct {
  uint64_t start;   /*!< start key index */
  uint64_t total;   /*!< total key index */
  uint8_t security; /*!< security level */
} address_opt_t;

/**
 * @brief Generates and returns addresses including an unused address.
 *
 * Generates and returns addresses by calling #iota_client_find_transactions until the first unused address is detected.
 *
 * @param[in] serv client service
 * @param[in] seed A seed for address generation.
 * @param[in] addr_opt address information containing security level and key indices
 * @param[out] out_addresses List of addresses including an unused address.
 * @return #retcode_t
 */
retcode_t iota_client_get_new_address(iota_client_service_t const* const serv, flex_trit_t const* const seed,
                                      address_opt_t const addr_opt, hash243_queue_t* out_addresses);

/**
 * @brief Check if the input address is an unused address
 *
 * @param[in] serv client service
 * @param[in] addr examining address
 * @param[in, out] is_unused return if addr has been unused
 * @param[in] with_txs If <b>True</b> return transactions of this address in the <b>transactions</b>.
 * @param[in, out] transactions if <b>with_tx_list</b> is true, return transaction list of this address, otherwise
 * ignore this parameter.
 * @return #retcode_t
 */
retcode_t is_unused_address(iota_client_service_t const* const serv, flex_trit_t const* const addr,
                            bool* const is_unused, bool with_txs, hash243_queue_t* transactions);

/**
 * @brief Returns an unspent address and the index.
 *
 * Returns an unspent address and the index.
 *
 * @param[in] serv client service
 * @param[in] seed A seed for address generation.
 * @param[in] addr_opt address information containing security level and key indices
 * @param[out] unspent_addr An unspent address.
 * @param[out] unspent_index The index of unspent address.
 * @return #retcode_t
 */
retcode_t iota_client_get_unspent_address(iota_client_service_t const* const serv, flex_trit_t const* const seed,
                                          address_opt_t const addr_opt, flex_trit_t* unspent_addr,
                                          uint64_t* unspent_index);

#ifdef __cplusplus
}
#endif

#endif  // CCLIENT_API_GET_NEW_ADDRESS_H

/** @} */