/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#ifndef __CONSENSUS_CONSENSUS_H__
#define __CONSENSUS_CONSENSUS_H__

#include "common/errors.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Initializes all consensus components
 *
 * @return a status code
 */
retcode_t iota_consensus_init();

/**
 * Starts all consensus components
 *
 * @return a status code
 */
retcode_t iota_consensus_start();

/**
 * Stops all consensus components
 *
 * @return a status code
 */
retcode_t iota_consensus_stop();

/**
 * Destroys all consensus components
 *
 * @return a status code
 */
retcode_t iota_consensus_destroy();

#ifdef __cplusplus
}
#endif

#endif  // __CONSENSUS_CONSENSUS_H__
