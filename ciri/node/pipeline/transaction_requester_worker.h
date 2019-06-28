/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#ifndef __NODE_PIPELINE_TRANSACTION_REQUESTER_WORKER_H__
#define __NODE_PIPELINE_TRANSACTION_REQUESTER_WORKER_H__

#include "ciri/node/pipeline/transaction_requester.h"
#include "common/errors.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Starts a transaction requester
 *
 * @param transaction_requester The transaction requester
 *
 * @return a status code
 */
retcode_t requester_start(transaction_requester_t *const transaction_requester);

/**
 * Stops a transaction requester
 *
 * @param transaction_requester The transaction requester
 *
 * @return a status code
 */
retcode_t requester_stop(transaction_requester_t *const transaction_requester);

#ifdef __cplusplus
}
#endif

#endif  //__NODE_PIPELINE_TRANSACTION_REQUESTER_WORKER_H__
