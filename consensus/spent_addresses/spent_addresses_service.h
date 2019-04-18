/*
 * Copyright (c) 2019 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#ifndef __CONSENSUS_SPENT_ADDRESSES_SPENT_ADDRESSES_SERVICE_H__
#define __CONSENSUS_SPENT_ADDRESSES_SPENT_ADDRESSES_SERVICE_H__

#include "common/errors.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct spent_addresses_service_s {
} spent_addresses_service_t;

/**
 * Initializes a spent addresses service
 *
 * @param sas The spent addresses service
 *
 * @return a status code
 */
retcode_t iota_spent_addresses_service_init(spent_addresses_service_t *const sas);

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
