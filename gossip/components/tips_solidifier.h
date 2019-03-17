/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#ifndef __GOSSIP_COMPONENTS_TIPS_SOLIDIFIER_H__
#define __GOSSIP_COMPONENTS_TIPS_SOLIDIFIER_H__

#include <stdbool.h>

#include "common/errors.h"
#include "gossip/conf.h"
#include "gossip/tips_cache.h"
#include "utils/handles/thread.h"

#ifdef __cplusplus
extern "C" {
#endif

// Forward declarations
typedef struct transaction_solidifier_s transaction_solidifier_t;

/**
 * A component that randomly select tips and tries to solidify them
 */
typedef struct tips_solidifier_s {
  iota_gossip_conf_t *conf;
  thread_handle_t thread;
  bool running;
  tips_cache_t *tips;
  transaction_solidifier_t *transaction_solidifier;
} tips_solidifier_t;

/**
 * Initializes a tips solidifier
 *
 * @param tips_solidifier The tips solidifier
 * @param conf A gossip configuration
 * @param tips A tips cache
 * @param transaction_solidifier A transaction solidifier
 *
 * @return a status code
 */
retcode_t tips_solidifier_init(tips_solidifier_t *const tips_solidifier, iota_gossip_conf_t *const conf,
                               tips_cache_t *const tips, transaction_solidifier_t *const transaction_solidifier);

/**
 * Starts a tips solidifier
 *
 * @param tips_solidifier The tips solidifier
 *
 * @return a status code
 */
retcode_t tips_solidifier_start(tips_solidifier_t *const tips_solidifier);

/**
 * Stops a tips solidifier
 *
 * @param tips_solidifier The tips solidifier
 *
 * @return a status code
 */
retcode_t tips_solidifier_stop(tips_solidifier_t *const tips_solidifier);

/**
 * Destroys a tips solidifier
 *
 * @param tips_solidifier The tips solidifier
 *
 * @return a status code
 */
retcode_t tips_solidifier_destroy(tips_solidifier_t *const tips_solidifier);

#ifdef __cplusplus
}
#endif

#endif  //__GOSSIP_COMPONENTS_TIPS_SOLIDIFIER_H__
