/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#ifndef __CIRI_API_API_H__
#define __CIRI_API_API_H__

#include <stdbool.h>
#include <stdint.h>

#include "cclient/serialization/serializer.h"
#include "ciri/api/conf.h"
#include "common/errors.h"
#include "consensus/tangle/tangle.h"
#include "consensus/transaction_validator/transaction_validator.h"
#include "gossip/components/broadcaster.h"
#include "utils/handles/thread.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * The IOTA API implementation
 * https://iota.readme.io/reference
 */
typedef struct iota_api_s {
  iota_api_conf_t conf;
  thread_handle_t thread;
  bool running;
  node_t *node;
  tangle_t *tangle;
  transaction_validator_t *transaction_validator;
  serializer_t serializer;
  serializer_type_t serializer_type;
} iota_api_t;

/**
 * Initializes an API
 *
 * @param api The API
 * @param node A node
 * @param tangle A tangle
 * @param transaction_validator A transaction validator
 * @param serializer_type A serializer type
 *
 * @return a status code
 */
retcode_t iota_api_init(iota_api_t *const api, node_t *const node,
                        tangle_t *const tangle,
                        transaction_validator_t *const transaction_validator,
                        serializer_type_t const serializer_type);

/**
 * Starts an API
 *
 * @param api The API
 *
 * @return a status code
 */
retcode_t iota_api_start(iota_api_t *const api);

/**
 * Stops an API
 *
 * @param api The API
 *
 * @return a status code
 */
retcode_t iota_api_stop(iota_api_t *const api);

/**
 * Destroys an API
 *
 * @param api The API
 *
 * @return a status code
 */
retcode_t iota_api_destroy(iota_api_t *const api);

/**
 * Returns information about your node.
 *
 * @param api The API
 * @param res The response
 *
 * @return a status code
 */
retcode_t iota_api_get_node_info(iota_api_t const *const api,
                                 get_node_info_res_t *const res);

/**
 * Returns the set of neighbors you are connected with, as well as their
 * activity count. The activity counter is reset after restarting IRI.
 *
 * @param api The API
 * @param res The response
 *
 * @return a status code
 */
retcode_t iota_api_get_neighbors(iota_api_t const *const api,
                                 get_neighbors_res_t *const res);

/**
 * Adds a list of neighbors to your node. It should be noted that this is only
 * temporary, and the added neighbors will be removed from your set of neighbors
 * after you relaunch IRI.
 *
 * @param api The API
 * @param req The request
 * @param res The response
 *
 * @return a status code
 */
retcode_t iota_api_add_neighbors(iota_api_t const *const api,
                                 add_neighbors_req_t const *const req,
                                 add_neighbors_res_t *const res);

/**
 * Removes a list of neighbors to your node. This is only temporary, and if you
 * have your neighbors added via the command line, they will be retained after
 * you restart your node.
 *
 * @param api The API
 * @param req The request
 * @param res The response
 *
 * @return a status code
 */
retcode_t iota_api_remove_neighbors(iota_api_t const *const api,
                                    remove_neighbors_req_t const *const req,
                                    remove_neighbors_res_t *const res);

/**
 * Returns the list of tips.
 *
 * @param api The API
 * @param res The response
 *
 * @return a status code
 */
retcode_t iota_api_get_tips(iota_api_t const *const api,
                            get_tips_res_t *const res);

/**
 * Finds the transactions which match the specified input and return. All input
 * values are lists, for which a list of return values (transaction hashes), in
 * the same order, is returned for all individual elements. The input fields can
 * either be bundles, addresses, tags or approvees. Using multiple of these
 * input fields returns the intersection of the values.
 *
 * @param api The API
 * @param req The request
 * @param res The response
 *
 * @return a status code
 */
retcode_t iota_api_find_transactions(iota_api_t const *const api,
                                     find_transactions_req_t const *const req,
                                     find_transactions_res_t *const res);

/**
 * Returns the raw transaction data (trytes) of a specific transaction. These
 * trytes can then be easily converted into the actual transaction object. See
 * utility functions for more details.
 *
 * @param api The API
 * @param req The request
 * @param res The response
 *
 * @return a status code
 */
retcode_t iota_api_get_trytes(iota_api_t const *const api,
                              get_trytes_req_t const *const req,
                              get_trytes_res_t *const res);

/**
 * Gets the inclusion states of a set of transactions. This is for determining
 * if a transaction was accepted and confirmed by the network or not. You can
 * search for multiple tips (and thus, milestones) to get past inclusion states
 * of transactions. This API call simply returns a list of boolean values in the
 * same order as the transaction list you submitted, thus you get a true/false
 * whether a transaction is confirmed or not.
 *
 * @param api The API
 * @param req The request
 * @param res The response
 *
 * @return a status code
 */
retcode_t iota_api_get_inclusion_states(
    iota_api_t const *const api, get_inclusion_state_req_t const *const req,
    get_inclusion_state_res_t *const res);

/**
 * Returns the confirmed balance, as viewed by tips, in case tips is not
 * supplied, the balance is based on the latest confirmed milestone. In addition
 * to the balances, it also returns the referencing tips (or milestone), as well
 * as the index with which the confirmed balance was determined. The balances is
 * returned as a list in the same order as the addresses were provided as input.
 *
 * @param api The API
 * @param req The request
 * @param res The response
 *
 * @return a status code
 */
retcode_t iota_api_get_balances(iota_api_t const *const api,
                                get_balances_req_t const *const req,
                                get_balances_res_t *const res);

/**
 * Tip selection which returns trunkTransaction and branchTransaction. The input
 * value depth determines how many milestones to go back to for finding the
 * transactions to approve. The higher your depth value, the more work you have
 * to do as you are confirming more transactions. If the depth is too large
 * (usually above 15, it depends on the node's configuration) an error will be
 * returned. The reference is an optional hash of a transaction you want to
 * approve. If it can't be found at the specified depth then an error will be
 * returned.
 *
 * @param api The API
 * @param req The request
 * @param res The response
 *
 * @return a status code
 */
retcode_t iota_api_get_transactions_to_approve(
    iota_api_t const *const api,
    get_transactions_to_approve_req_t const *const req,
    get_transactions_to_approve_res_t *const res);

/**
 * Attaches the specified transactions (trytes) to the Tangle by doing Proof of
 * Work. You need to supply branchTransaction as well as trunkTransaction
 * (basically the tips which you're going to validate and reference with this
 * transaction) - both of which you'll get through the getTransactionsToApprove
 * API call. The returned value is a different set of tryte values which you can
 * input into broadcastTransactions and storeTransactions. The returned tryte
 * value, the last 243 trytes basically consist of the: trunkTransaction +
 * branchTransaction + nonce. These are valid trytes which are then accepted by
 * the network.
 *
 * @param api The API
 * @param req The request
 * @param res The response
 *
 * @return a status code
 */
retcode_t iota_api_attach_to_tangle(iota_api_t const *const api,
                                    attach_to_tangle_req_t const *const req,
                                    attach_to_tangle_res_t *const res);

/**
 * Interrupts and completely aborts the attachToTangle process.
 *
 * @param api The API
 *
 * @return a status code
 */
retcode_t iota_api_interrupt_attaching_to_tangle(iota_api_t const *const api);

/**
 * Broadcasts a list of transactions to all neighbors. The input trytes for this
 * call are provided by attachToTangle.
 *
 * @param api The API
 * @param req The request
 *
 * @return a status code
 */
retcode_t iota_api_broadcast_transactions(
    iota_api_t const *const api, broadcast_transactions_req_t const *const req);

/**
 * Stores transactions into the local storage. The trytes to be used for this
 * call are returned by attachToTangle.
 *
 * @param api The API
 * @param req The request
 *
 * @return a status code
 */
retcode_t iota_api_store_transactions(
    iota_api_t const *const api, store_transactions_req_t const *const req);

/**
 * Checks if a list of addresses was ever spent from, in the current epoch, or
 * in previous epochs.
 *
 * @param api The API
 * @param req The request
 * @param res The response
 *
 * @return a status code
 */
retcode_t iota_api_were_addresses_spent_from(
    iota_api_t const *const api, check_consistency_req_t const *const req,
    check_consistency_res_t *const res);

retcode_t iota_api_check_consistency(iota_api_t const *const api,
                                     check_consistency_req_t const *const req,
                                     check_consistency_res_t *const res);

#ifdef __cplusplus
}
#endif

#endif
