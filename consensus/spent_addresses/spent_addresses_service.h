/*
 * Copyright (c) 2019 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#ifndef __CONSENSUS_SPENT_ADDRESSES_SPENT_ADDRESSES_SERVICE_H__
#define __CONSENSUS_SPENT_ADDRESSES_SPENT_ADDRESSES_SERVICE_H__

#include "common/errors.h"
#include "consensus/conf.h"
#include "consensus/spent_addresses/spent_addresses_provider.h"
#include "consensus/tangle/tangle.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct spent_addresses_service_s {
  iota_consensus_conf_t *conf;
} spent_addresses_service_t;

/**
 * Initializes a spent addresses service
 * Reads previous spent addresses from files
 *
 * @param sas The spent addresses service
 * @param conf Consensus configuration
 *
 * @return a status code
 */
retcode_t iota_spent_addresses_service_init(spent_addresses_service_t *const sas, iota_consensus_conf_t *const conf);

/**
 * Destroys a spent addresses service
 *
 * @param sas The spent addresses service
 *
 * @return a status code
 */
retcode_t iota_spent_addresses_service_destroy(spent_addresses_service_t *const sas);

#ifdef __cplusplus
}
#endif

#endif  // __CONSENSUS_SPENT_ADDRESSES_SPENT_ADDRESSES_SERVICE_H__
