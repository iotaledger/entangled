/*
 * Copyright (c) 2019 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#ifndef __CONSENSUS_SPENT_ADDRESSES_SPENT_ADDRESSES_H__
#define __CONSENSUS_SPENT_ADDRESSES_SPENT_ADDRESSES_H__

#include "common/errors.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Initializes spent addresses
 *
 * @return a status code
 */
retcode_t iota_spent_addresses_init();

/**
 * Destroys spent addresses
 *
 * @return a status code
 */
retcode_t iota_spent_addresses_destroy();

#ifdef __cplusplus
}
#endif

#endif  // __CONSENSUS_SPENT_ADDRESSES_SPENT_ADDRESSES_H__
