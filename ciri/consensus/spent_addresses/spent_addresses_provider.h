/*
 * Copyright (c) 2019 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#ifndef __CONSENSUS_SPENT_ADDRESSES_SPENT_ADDRESSES_PROVIDER_H__
#define __CONSENSUS_SPENT_ADDRESSES_SPENT_ADDRESSES_PROVIDER_H__

#include "common/errors.h"
#include "common/storage/storage.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct spent_addresses_provider_s {
  storage_connection_t connection;
} spent_addresses_provider_t;

/**
 * Initializes a spent addresses provider
 *
 * @param[out]  sap   The spent addresses provider
 * @param[in]   conf  A connection configuration
 *
 * @return a status code
 */
retcode_t iota_spent_addresses_provider_init(spent_addresses_provider_t *const sap,
                                             storage_connection_config_t const *const conf);

/**
 * Destroys a spent addresses provider
 *
 * @param[out] sap The spent addresses provider
 *
 * @return a status code
 */
retcode_t iota_spent_addresses_provider_destroy(spent_addresses_provider_t *const sap);

/**
 * Stores a spent address in a provider
 *
 * @param[in] sap     The spent addresses provider
 * @param[in] address The spent address
 *
 * @return a status code
 */
retcode_t iota_spent_addresses_provider_store(spent_addresses_provider_t const *const sap,
                                              flex_trit_t const *const address);

/**
 * Stores spent addresses in a provider
 *
 * @param[in] sap       The spent addresses provider
 * @param[in] addresses The spent addresses
 *
 * @return a status code
 */
retcode_t iota_spent_addresses_provider_batch_store(spent_addresses_provider_t const *const sap,
                                                    hash243_set_t const addresses);
/**
 * Checks if a spent address exists in a provider
 *
 * @param[in]   sap     The spent addresses provider
 * @param[in]   address The spent address
 * @param[out]  exist   Whether the spent address exists or not
 *
 * @return a status code
 */
retcode_t iota_spent_addresses_provider_exist(spent_addresses_provider_t const *const sap,
                                              flex_trit_t const *const address, bool *const exist);

/**
 * Imports spent addresses from a file
 *
 * @param[in] sap   The spent addresses provider
 * @param[in] file  The file
 *
 * @return a status code
 */
retcode_t iota_spent_addresses_provider_import(spent_addresses_provider_t const *const sap, char const *const file);

#ifdef __cplusplus
}
#endif

#endif  // __CONSENSUS_SPENT_ADDRESSES_SPENT_ADDRESSES_PROVIDER_H__
