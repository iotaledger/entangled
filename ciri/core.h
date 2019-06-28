/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#ifndef __CIRI_CORE_H__
#define __CIRI_CORE_H__

#include "ciri/conf.h"
#include "ciri/consensus/consensus.h"
#include "ciri/node/node.h"
#include "ciri/node/pipeline/transaction_requester.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct core_s {
  bool running;
  iota_ciri_conf_t conf;
  iota_consensus_t consensus;
  iota_node_t node;
} core_t;

/**
 * Initializes a core
 *
 * @param core The core
 * @param tangle A tangle
 *
 * @return a status code
 */
retcode_t core_init(core_t *const core, tangle_t *const tangle);

/**
 * Starts a core
 *
 * @param core The core
 * @param tangle A tangle
 *
 * @return a status code
 */
retcode_t core_start(core_t *const core, tangle_t *const tangle);

/**
 * Stops a core
 *
 * @param core The core
 *
 * @return a status code
 */
retcode_t core_stop(core_t *const core);

/**
 * Destroys a core
 *
 * @param core The core
 *
 * @return a status code
 */
retcode_t core_destroy(core_t *const core);

#ifdef __cplusplus
}
#endif

#endif  // __CIRI_CORE_H__
