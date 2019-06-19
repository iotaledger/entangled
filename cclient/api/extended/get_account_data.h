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

#include "cclient/api/extended/types.h"
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

void account_data_init(account_data_t* const account);

void account_data_clear(account_data_t* const account);
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