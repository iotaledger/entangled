/*
 * Copyright (c) 2019 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#ifndef __CONSENSUS_SPENT_ADDRESSES_SPENT_ADDRESSES_SERVICE_H__
#define __CONSENSUS_SPENT_ADDRESSES_SPENT_ADDRESSES_SERVICE_H__

#include "ciri/consensus/conf.h"
#include "ciri/consensus/spent_addresses/spent_addresses_provider.h"
#include "ciri/consensus/tangle/tangle.h"
#include "common/errors.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct spent_addresses_service_s {
  iota_consensus_conf_t *conf;
} spent_addresses_service_t;

/**
 * Initializes a spent addresses service
 *
 * @param[out]  sas   The spent addresses service
 * @param[in]   conf  Consensus configuration
 *
 * @return a status code
 */
retcode_t iota_spent_addresses_service_init(spent_addresses_service_t *const sas, iota_consensus_conf_t *const conf);

/**
 * Destroys a spent addresses service
 *
 * @param[out] sas The spent addresses service
 *
 * @return a status code
 */
retcode_t iota_spent_addresses_service_destroy(spent_addresses_service_t *const sas);

/**
 * Checks whether an address is associated with a valid signed output
 *
 * @param[in]   sas     The spent addresses service
 * @param[in]   sap     A spent addresses provider
 * @param[in]   tangle  A tangle
 * @param[in]   address The address
 * @param[out]  spent   True if the address was spent from, false otherwise
 *
 * @return a status code
 */
retcode_t iota_spent_addresses_service_was_address_spent_from(spent_addresses_service_t const *const sas,
                                                              spent_addresses_provider_t const *const sap,
                                                              tangle_t const *const tangle,
                                                              flex_trit_t const *const address, bool *const spent);

/**
 * Checks whether a transaction is associated with a valid signed output
 *
 * @param[in]   tangle  A tangle
 * @param[in]   tx The Transaction
 * @param[in]   hash The hash
 * @param[out]  spent   True if the address was spent from, false otherwise
 *
 * @return a status code
 */
retcode_t iota_spent_addresses_service_was_tx_spent_from(tangle_t const *const tangle,
                                                         iota_transaction_t const *const tx,
                                                         flex_trit_t const *const hash, bool *const spent);

#ifdef __cplusplus
}
#endif

#endif  // __CONSENSUS_SPENT_ADDRESSES_SPENT_ADDRESSES_SERVICE_H__
