/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#ifndef __GOSSIP_COMPONENTS_TRANSACTION_REQUESTER_WORKER_H__
#define __GOSSIP_COMPONENTS_TRANSACTION_REQUESTER_WORKER_H__

#include "common/errors.h"
#include "gossip/components/transaction_requester.h"

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

#endif  //__GOSSIP_COMPONENTS_TRANSACTION_REQUESTER_WORKER_H__
