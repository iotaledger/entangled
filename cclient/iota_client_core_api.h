/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#ifndef CCLIENT_IOTA_CORE_API_H_
#define CCLIENT_IOTA_CORE_API_H_

#include "cclient/service.h"
#include "request/requests.h"
#include "response/responses.h"
#include "utils/logger_helper.h"

#ifdef __cplusplus
extern "C" {
#endif

retcode_t iota_client_core_init(iota_client_service_t* const serv);
void iota_client_core_destroy(iota_client_service_t* const serv);

/**
 * Returns information about your node.
 *
 * https://iota.readme.io/reference#getnodeinfo
 *
 * @param service IRI node end point.
 * @param res Response containing the node's info
 *
 * @return error value.
 */

retcode_t iota_client_get_node_info(const iota_client_service_t* const service,
                                    get_node_info_res_t** res);

/**
 * Returns the set of neighbors service node is connected with, as well as their
 * activity counters regarding participation in the protocol. The activity
 * counters are reset after restarting IRI.
 *
 * https://iota.readme.io/reference#getneighborsactivity
 *
 * @param service IRI node end point.
 * @param res Response containing the neighbors' info
 *
 * @return error value.
 */
retcode_t iota_client_get_neighbors(const iota_client_service_t* const service,
                                    get_neighbors_res_t* const res);

/**
 * Add a list of neighbors to iri node. It should be noted that this is only
 * temporary, and the added neighbors will be removed from your set of neighbors
 * after you relaunch IRI.
 *
 * https://iota.readme.io/reference#addneighors
 *
 * @param service IRI node end point.
 * @param req Request containing list of neighbors to add
 * @param res Response containing a number of neighbor
 *
 * @return error value.
 */
retcode_t iota_client_add_neighbors(const iota_client_service_t* const service,
                                    const add_neighbors_req_t* const req,
                                    add_neighbors_res_t* res);

/**
 * Removes a list of neighbors from your iri node. This is only temporary, and
 * if you have your neighbors added via the command line, they will be retained
 * after you restart your node.
 *
 * https://iota.readme.io/reference#removeneighbors

 * @param service IRI node end point.
 * @param req Request containing list of neighbors to remove.
 * @param res Response containing a number of neighbor
 *
 * @return error value.
 */
retcode_t iota_client_remove_neighbors(
    const iota_client_service_t* const service,
    const remove_neighbors_req_t* const req, remove_neighbors_res_t* res);

/**
 * Returns the list of tips.
 *
 * https://iota.readme.io/reference#gettips
 *
 * @param service IRI node end point.
 * @param res Response containing the tips
 *
 * @return error value.
 */
retcode_t iota_client_get_tips(const iota_client_service_t* const service,
                               get_tips_res_t* res);
/**
 * Find the transactions which attach the specified input and return. All input
 * values are lists (in req), for which a list of return values (transaction
 * hashes) in the same order is returned for all individual elements. The input
 * fields can either be bundles, addresses, tags or approvees. Using multiple of
 * these input fields returns the intersection of the values.
 *
 * https://iota.readme.io/reference#findtransactions
 *
 * @param service IRI node end point.
 * @param req - Request containing all elements by which transactions should be
 * sought.
 * @param res Response containing found transactions
 *
 * @return error value.
 */
retcode_t iota_client_find_transactions(
    iota_client_service_t const* const service,
    find_transactions_req_t const* const req,
    find_transactions_res_t* const res);

/**
 * Returns the raw transaction data (trytes) of a specific transaction. These
 * trytes can then be easily converted into the actual transaction object. See
 * utility functions for more details.
 *
 * https://iota.readme.io/reference#gettrytes
 *
 * @param service IRI node end point.
 * @param req Request containing hashes of transactions to which trytes are
 * to be retrieved
 * @param res Response containing trytes of specific transactions
 *
 * @return error value.
 */
retcode_t iota_client_get_trytes(const iota_client_service_t* const service,
                                 get_trytes_req_t* const req,
                                 get_trytes_res_t* res);

/**
 * Get the inclusion states of a set of transactions. This is for determining if
 * a transaction was accepted and confirmed by the network or not. You can
 * search for multiple tips (and thus, milestones) to get past inclusion states
 * of transactions. This API call simply returns a list of boolean values in the
 * same order as the transaction list you submitted, thus you get a true/false
 * whether a transaction is confirmed or not.
 *
 * https://iota.readme.io/reference#getinclusionstates
 *
 * @param service IRI node end point.
 * @param req Request containing transactions to which we are searching for
 * inclusion state for.
 * @param res Response containing the corresponding inclusion states
 *
 * @return error value.
 */
retcode_t iota_client_get_inclusion_states(
    const iota_client_service_t* const service,
    get_inclusion_state_req_t* const req, get_inclusion_state_res_t* res);

/**
 * Similar to iota_client_get_inclusion_states. It returns the confirmed balance
 * which a list of addresses have at the latest confirmed milestone. In addition
 * to the balances, it also returns the milestone as well as the index with
 * which the confirmed balance was determined. The balances is returned as a
 * list in the same order as the addresses were provided as input.
 *
 * https://iota.readme.io/reference#getbalances
 *
 * @param service IRI node end point.
 * @param req Request containing the addresses for which balances are
 * requested
 * @param res Response containing the requested balances
 *
 * @return error value.
 */
retcode_t iota_client_get_balances(iota_client_service_t const* const service,
                                   get_balances_req_t const* const req,
                                   get_balances_res_t* res);

/**
 * Tip selection which returns trunkTransaction and branchTransaction. The input
 * value is depth, which basically determines how many bundles to go back to for
 * finding the transactions to approve. The higher your depth value, the more
 * "babysitting" you do for the network (as you have to confirm more
 * transactions).
 *
 * https://iota.readme.io/reference#gettransactionstoapprove
 *
 * @param service IRI node end point.
 * @param req Request containing number of bundles to go back to determine
 * the transactions for approval.
 * @param res Response containing transactions to approve (branch + trunk)
 *
 * @return error value.
 */
retcode_t iota_client_get_transactions_to_approve(
    const iota_client_service_t* const service,
    const get_transactions_to_approve_req_t* const req,
    get_transactions_to_approve_res_t** res);

/**
 * Attaches the specified transactions (trytes) to the Tangle by doing Proof of
 * Work. You need to supply branchTransaction as well as trunkTransaction
 * (basically the tips which you're going to validate and reference with this
 * transaction) - both of which you'll get through the
 * iota_client_get_transactions_to_approve API call. The returned value is a
 * different set of tryte values which you can input into
 * iota_client_broadcast_transactions and iota_client_store_transactions. The
 * returned tryte value, the last 243 trytes basically consist of the:
 * trunkTransaction + branchTransaction + nonce. These are valid trytes which
 * are then accepted by the network.
 *
 * https://iota.readme.io/reference#attachtotangle
 *
 *
 * @param service IRI node end point.
 * @param req Request containing the attachment params
 * @param res Response containing trytes
 *
 * @return error value.
 */
retcode_t iota_client_attach_to_tangle(
    const iota_client_service_t* const service,
    const attach_to_tangle_req_t* const req, attach_to_tangle_res_t** res);

/**
 * Interrupts and completely aborts the iota_client_attach_to_tangle process.
 *
 * https://iota.readme.io/reference#interruptattachingtotangle
 *
 * @param service IRI node end point.
 *
 * @return error value.
 */

retcode_t iota_client_interrupt_attaching_to_tangle(
    const iota_client_service_t* const service);

/**
 * Broadcast a list of transactions to all neighbors. The input trytes for this
 * call are provided by iota_client_attach_to_tangle.
 *
 * https://iota.readme.io/reference#broadcasttransactions
 *
 * @param service IRI node end point.
 * @param req - Request containing the transactions to broadcast
 *
 * @return error value.
 */
retcode_t iota_client_broadcast_transactions(
    const iota_client_service_t* const service,
    broadcast_transactions_req_t* req);

/**
 * Store transactions into the local storage. The trytes to be used for this
 * call are returned by iota_client_attach_to_tangle.
 *
 * https://iota.readme.io/reference#storetransactions
 *
 * @param service IRI node end point.
 * @param req - Request containing the transactions to store
 *
 * @return The error value.
 */
retcode_t iota_client_store_transactions(
    const iota_client_service_t* const service,
    store_transactions_req_t* const req);

/**
 * Checks if a transaction is _consistent_ or a set of transactions are
 * _co-consistent_.
 * As long as a transaction is consistent it might be accepted
 * by the network. In case transaction is inconsistent, it will not be accepted,
 * and a reattachment is required by calling iota_client_replay_bundle().
 *
 * https://iota.readme.io/reference#checkconsistency
 *
 * @param service IRI node end point.
 * @param req - Request containing tail transaction hash (hash of
 * transaction with `currentIndex=0`), or array of tail transaction hashes.
 * @param res - Response containing consistency state of given transaction
 * or co-consistency of given transactions.
 *
 * @return The error value.
 */
retcode_t iota_client_check_consistency(
    const iota_client_service_t* const service,
    check_consistency_req_t* const req, check_consistency_res_t* res);

#ifdef __cplusplus
}
#endif

#endif  // CCLIENT_IOTA_CORE_API_H_
