/*
 * Copyright (c) 2019 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#ifndef __CONSENSUS_SPENT_ADDRESSES_SPENT_ADDRESSES_PROVIDER_H__
#define __CONSENSUS_SPENT_ADDRESSES_SPENT_ADDRESSES_PROVIDER_H__

#include "common/errors.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct spent_addresses_provider_s {
} spent_addresses_provider_t;

/**
 * Initializes a spent addresses provider
 *
 * @return a status code
 */
retcode_t iota_spent_addresses_provider_init(spent_addresses_provider_t* const sap);

/**
 * Destroys a spent addresses provider
 *
 * @return a status code
 */
retcode_t iota_spent_addresses_provider_destroy(spent_addresses_provider_t* const sap);

#ifdef __cplusplus
}
#endif

#endif  // __CONSENSUS_SPENT_ADDRESSES_SPENT_ADDRESSES_PROVIDER_H__
