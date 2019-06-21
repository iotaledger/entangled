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
#ifndef CCLIENT_API_GET_ACCOUNT_DATA_H
#define CCLIENT_API_GET_ACCOUNT_DATA_H

#include "cclient/http/http.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Account information
 *
 */
typedef struct {
  uint64_t balance;                               /*!< total balance */
  flex_trit_t latest_address[FLEX_TRIT_SIZE_243]; /*!< unused address */
  hash243_queue_t addresses;                      /*!< List of used addresses */
  hash243_queue_t transactions;                   /*!< List of transactions */
  UT_array* balances;                             /*!< List of balances */
} account_data_t;

/**
 * @brief Initialize the #account_data_t structure.
 *
 * @param[out] account An #account_data_t object.
 */
void account_data_init(account_data_t* const account);

/**
 * @brief Clean up the #account_data_t.
 *
 * @param[out] account An #account_data_t object.
 */
void account_data_clear(account_data_t* const account);

/**
 * @brief Gets the balance value by index.
 *
 * @param[in] account the #account_data_t object.
 * @param[in] index The index of the balance list.
 * @return the balance of the index in #account_data_t.balances.
 */
uint64_t account_data_get_balance(account_data_t* const account, size_t index);

/**
 * @brief Returns an #account_data_t object.
 *
 * Returns an #account_data_t object, containing account information about <b>addresses</b>, <b>transactions</b>,
 * <b>inputs</b> and total account balance.
 *
 * @param[in] serv client service
 * @param[in] seed seed
 * @param[in] security Security level to be used for getting addresses
 * @param[out] out_account the result containing a unused address, used addresses, transactions, and total balance.
 * @return #retcode_t
 */
retcode_t iota_client_get_account_data(iota_client_service_t const* const serv, flex_trit_t const* const seed,
                                       uint8_t security, account_data_t* out_account);

#ifdef __cplusplus
}
#endif

#endif  // CCLIENT_API_GET_ACCOUNT_DATA_H

/** @} */