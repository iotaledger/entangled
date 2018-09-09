/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#ifndef CCLIENT_IOTA_API_H_
#define CCLIENT_IOTA_API_H_

#include "cclient/service.h"
#include "request/requests.h"
#include "response/responses.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Returns information about your node.
 *
 * https://iota.readme.io/reference#getnodeinfo
 *
 * @return The response.
 */

iota_api_result_t iota_api_get_node_info(
    const iota_http_service_t* const service, get_node_info_res_t* const res);

/**
 * Returns the set of neighbors service node is connected with, as well as their
 * activity counters regarding participation in the protocol. The activity
 * counters are reset after restarting IRI.
 *
 * https://iota.readme.io/reference#getneighborsactivity
 *
 * @param service IRI node end point.
 * @param res get_node_info_res_t struct containing the node's info
 *
 * @return error value.
 */
iota_api_result_t iota_api_get_neighbors(
    const iota_http_service_t* const service, get_neighbors_res_t* const res);

/**
 * Add a list of neighbors to iri node. It should be noted that this is only
 * temporary, and the added neighbors will be removed from your set of neighbors
 * after you relaunch IRI.
 *
 * https://iota.readme.io/reference#addneighors
 *
 * @param service IRI node end point.
 * @param req Request containing list of neighbors to add
 *
 * @return error value.
 */
iota_api_result_t iota_api_add_neighbors(
    const iota_http_service_t* const service, add_neighbors_req_t* req,
    add_neighbors_res_t* res);

/**
 * Removes a list of neighbors from your iri node. This is only temporary, and
 if you
 * have your neighbors added via the command line, they will be retained after
 * you restart your node.
 *
 * https://iota.readme.io/reference#reoveneighors

 * @param service IRI node end point.
 * @param uris List of URI elements.
 * @param nuUris Number of elements in uris
 *
 * @return error value.
 */
iota_api_result_t iota_api_remove_neighbors(
    const iota_http_service_t* const service, remove_neighbors_req_t* req,
    remove_neighbors_res_t* res);

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
iota_api_result_t iota_api_get_tips(const iota_http_service_t* const service,
                                    get_tips_res_t* const res);
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
 * @param req - find_transactions_req_t struct containing all elements by which
 * transactions should be sought.
 * @param res Response containing found transactions
 *
 * @return error value.
 */
iota_api_result_t iota_api_find_transactions(
    const iota_http_service_t* const service,
    const find_transactions_req_t* const req,
    find_transactions_res_t* const res);

/**
 * Returns the raw transaction data (trytes) of a specific transaction. These
 * trytes can then be easily converted into the actual transaction object. See
 * utility functions for more details.
 *
 * https://iota.readme.io/reference#gettrytes
 *
 * @param service IRI node end point.
 * @param req The request containing Hashes of transactions to which trytes are
 * to be retrieved
 *
 * @return error value.
 */
iota_api_result_t iota_api_get_trytes(const iota_http_service_t* const service,
                                      get_trytes_req_t* req,
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
 * @param req The request with transactions to which we are searching for
 * inclusion state for.
 * @param res The response with the corresponding inclusion states
 *
 * @return error value.
 */
iota_api_result_t iota_api_get_inclusion_states(
    const iota_http_service_t* const service, get_inclusion_state_req_t* req,
    get_inclusion_state_res_t* res);

/**
 * Similar to iota_api_get_inclusion_states. It returns the confirmed balance
 * which a list of addresses have at the latest confirmed milestone. In addition
 * to the balances, it also returns the milestone as well as the index with
 * which the confirmed balance was determined. The balances is returned as a
 * list in the same order as the addresses were provided as input.
 *
 * https://iota.readme.io/reference#getbalances
 *
 * @param service IRI node end point.
 * @param req The request containing the addresses for which balances are
 * requested
 * @param res Response containing the requested balances
 *
 * @return error value.
 */
iota_api_result_t iota_api_get_balances(
    const iota_http_service_t* const service,
    const get_balances_req_t* const req, get_balances_res_t* res);

/**
 * Tip selection which returns trunkTransaction and branchTransaction. The input
 * value is depth, which basically determines how many bundles to go back to for
 * finding the transactions to approve. The higher your depth value, the more
 * "babysitting" you do for the network (as you have to confirm more
 * transactions).
 *
 * https://iota.readme.io/reference#getbalances
 *
 * @param service IRI node end point.
 * @param depth Nuber of bundles to go back to determine the transactions for
 * approval.
 * @param res Transactions to approve (branch + trunk)
 *
 * @return error value.
 */
iota_api_result_t iota_api_get_transactions_to_approve(
    const iota_http_service_t* const service,
    get_transactions_to_approve_req_t* req,
    get_transactions_to_approve_res_t* res);

/**
 * Attaches the specified transactions (trytes) to the Tangle by doing Proof of
 * Work. You need to supply branchTransaction as well as trunkTransaction
 * (basically the tips which you're going to validate and reference with this
 * transaction) - both of which you'll get through the
 * iota_api_get_transactions_to_approve API call. The returned value is a
 * different set of tryte values which you can input into
 * iota_api_broadcast_transactions and iota_api_store_transactions. The returned
 * tryte value, the last 243 trytes basically consist of the: trunkTransaction +
 * branchTransaction + nonce. These are valid trytes which are then accepted by
 * the network.
 *
 * https://iota.readme.io/reference#attachtotangle
 *
 *
 * @param service IRI node end point.
 * @param req The request with the attachment params
 *
 * @return error value.
 */
iota_api_result_t iota_api_attach_to_tangle(
    const iota_http_service_t* const service, attach_to_tangle_req_t* req,
    attach_to_tangle_res_t* res);

/**
 * Interrupts and completely aborts the iota_api_attach_to_tangle process.
 *
 * https://iota.readme.io/reference#interruptattachingtotangle
 *
 * @param service IRI node end point.
 * @return error value.
 */

iota_api_result_t iota_api_interrupt_attaching_to_tangle(
    const iota_http_service_t* const service);

/**
 * Broadcast a list of transactions to all neighbors. The input trytes for this
 * call are provided by iota_api_attach_to_tangle.
 *
 * https://iota.readme.io/reference#broadcasttransactions
 *
 * @param service IRI node end point.
 * @param req - the request containing the transactions to broadcast
 *
 * @return error value.
 */
iota_api_result_t iota_api_broadcast_transactions(
    const iota_http_service_t* const service,
    broadcast_transactions_req_t* req);

/**
 * Store transactions into the local storage. The trytes to be used for this
 * call are returned by iota_api_attach_to_tangle.
 *
 * https://iota.readme.io/reference#storetransactions
 *
 * @param service IRI node end point.
 * @param req - the request containing the transactions to store
 *
 * @return The error value.
 */
iota_api_result_t iota_api_store_transactions(
    const iota_http_service_t* const service, store_transactions_req_t* req);

/**
 * Check if a list of addresses was ever spent from, in the current epoch, or in
 * previous epochs.
 *
 * https://iota.readme.io/reference#wereaddressesspentfrom
 *
 * @param service IRI node end point.
 * @param req - the request containing the addresses to check if they were ever
 * spent from.
 *
 * @return The error value.
 */
iota_api_result_t iota_api_were_addresses_spent_from(
    const iota_http_service_t* const service,
    were_addresses_spent_from_req_t* req, were_addresses_spent_from_res_t* res);
#ifdef __cplusplus
}
#endif

#endif  // CCLIENT_IOTA_API_H_
