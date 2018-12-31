/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#ifndef __PERCEPTIVE_NODE_PERCEPTIVE_NODE_H__
#define __PERCEPTIVE_NODE_PERCEPTIVE_NODE_H__

#include <stdbool.h>
#include "ciri/perceptive_node/conf.h"
#include "common/errors.h"
#include "common/trinary/flex_trit.h"
#include "consensus/consensus.h"
#include "utils/handles/thread.h"
#include "utils/logger_helper.h"

#include "utarray.h"

/**
 * The IOTA Perceptive node implementation
 */

typedef struct iota_perceptive_node_s {
  iota_perceptive_node_conf_t conf;
  thread_handle_t thread;
  bool running;
  iota_consensus_t *consensus;
} iota_perceptive_node_t;

#ifdef __cplusplus
extern "C" {
/**
 * Initializes a Perceptive Node
 *
 * @param pn The Perceptive Node
 * @param consensus A consensus
 *
 * @return a status code
 */
retcode_t iota_perceptive_node_init(iota_perceptive_node_t *const pn,
                                    iota_consensus_t const *const consensus);

/**
 * Starts a Perceptive Node
 *
 * @param pn The Perceptive Node
 *
 * @return a status code
 */
retcode_t iota_perceptive_node_start(iota_perceptive_node_t *const pn);

/**
 * Stops a Perceptive Node
 *
 * @param pn The Perceptive Node
 *
 * @return a status code
 */
retcode_t iota_perceptive_node_stop(iota_perceptive_node_t *const pn);

/**
 * Destroys a Perceptive Node
 *
 * @param pn The Perceptive Node
 *
 * @return a status code
 */
retcode_t iota_perceptive_node_destroy(iota_perceptive_node_t *const pn);
};
#endif

#endif  //__PERCEPTIVE_NODE_PERCEPTIVE_NODE_H__
