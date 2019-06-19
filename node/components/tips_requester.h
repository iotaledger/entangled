/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#ifndef __NODE_COMPONENTS_TIPS_REQUESTER_H__
#define __NODE_COMPONENTS_TIPS_REQUESTER_H__

#include <stdbool.h>

#include "common/errors.h"
#include "utils/handles/cond.h"
#include "utils/handles/thread.h"

// Forward declarations
typedef struct node_s node_t;
typedef struct tangle_s tangle_t;

typedef struct tips_requester_s {
  thread_handle_t thread;
  bool running;
  node_t *node;
  cond_handle_t cond;
} tips_requester_t;

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Initializes a tips requester
 *
 * @param tips_requester The tips requester state
 * @param node A node
 * @param tangle A tangle
 *
 * @return a status code
 */
retcode_t tips_requester_init(tips_requester_t *const tips_requester, node_t *const node);

/**
 * Starts a tips requester
 *
 * @param tips_requester The tips requester state
 *
 * @return a status code
 */
retcode_t tips_requester_start(tips_requester_t *const tips_requester);

/**
 * Stops a tips requester
 *
 * @param tips_requester The tips requester state
 *
 * @return a status code
 */
retcode_t tips_requester_stop(tips_requester_t *const tips_requester);

/**
 * Destroys a tips requester
 *
 * @param tips_requester The tips requester state
 *
 * @return a status code
 */
retcode_t tips_requester_destroy(tips_requester_t *const tips_requester);

#ifdef __cplusplus
}
#endif

#endif  //__NODE_COMPONENTS_TIPS_REQUESTER_H__
