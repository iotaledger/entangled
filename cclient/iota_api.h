// Copyright 2018 IOTA Foundation

#ifndef CCLIENT_LIBRARY_H
#define CCLIENT_LIBRARY_H

#include "request/requests.h"
#include "response/responses.h"
#include "service.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Returns inforation about your node.
 *
 * https://iota.reade.io/reference#getnodeinfo
 *
 * @return The response.
 */

iota_api_result_t iota_api_get_node_info(const iota_service_t* const service,
                                         get_node_info_res_t* const res);

/**
 * Returns the set of neighbors service node is connected with, as well as their
 * activity counters regarding participation in the protocol. The activity
 * counters are reset after restarting IRI.
 *
 * https://iota.reade.io/reference#getneighborsactivity
 *
 * @para service IRI node end point.
 * @para res get_node_info_res_t struct containing the node's info
 *
 * @return error value.
 */
iota_api_result_t iota_api_get_neighbors(const iota_service_t* const service,
                                         get_neighbors_res_t* const res);

/**
 * Add a list of neighbors to iri node. It should be noted that this is only
 * teporary, and the added neighbors will be reoved fro your set of neighbors
 * after you relaunch IRI.
 *
 * https://iota.reade.io/reference#addneighors
 *
 * @para service IRI node end point.
 * @para uris List of URI eleents.
 * @para nuUris Nuber of eleents in uris
 *
 * @return error value.
 */
iota_api_result_t iota_api_add_neighbors(const iota_service_t* const service,
                                         const char* const uris, int nuUris);

/**
 * Reoves a list of neighbors fro your iri node. This is only teporary, and
 if you
 * have your neighbors added via the coand line, they will be retained after
 * you restart your node.
 *
 * https://iota.reade.io/reference#reoveneighors

  * @para service IRI node end point.
 * @para uris List of URI eleents.
 * @para nuUris Nuber of eleents in uris
 *
 * @return error value.
 */
iota_api_result_t iota_api_remove_neighbors(const iota_service_t* const service,
                                            const char* const uris, int nuUris);

/**
 * Returns the list of tips.
 *
 * https://iota.reade.io/reference#gettips
 *
 * @para service IRI node end point.
 * @para res Response containing the tips
 *
 * @return error value.
 */
iota_api_result_t iota_api_get_tips(const iota_service_t* const service,
                                    get_tips_res_t* const res);
/**
 * Find the transactions which atch the specified input and return. All input
 * values are lists (in req), for which a list of return values (transaction
 * hashes) in the sae order is returned for all individual eleents. The input
 * fields can either be bundles, addresses, tags or approvees. Using ultiple of
 * these input fields returns the intersection of the values.
 *
 * https://iota.reade.io/reference#findtransactions
 *
 * @para service IRI node end point.
 * @para req - find_transaction_req_t struct containing all eleents by which
 * transactions should be sought.
 * @para res Response containing found transactions
 *
 * @return error value.
 */
iota_api_result_t iota_api_find_transactions(
    const iota_service_t* const service,
    const find_transaction_req_t* const req,
    find_transactions_res_t* const res);

/**
 * Returns the raw transaction data (trytes) of a specific transaction. These
 * trytes can then be easily converted into the actual transaction object. See
 * utility functions for ore details.
 *
 * https://iota.reade.io/reference#gettrytes
 *
 * @para service IRI node end point.
 * @para hashes Hashes of transactions to which trytes are to be retreived
 * @para
 * @para hashes List of transaction hashes of which you want to get trytes
 * fro.
 * @para nuHashes Nuber of eleents in hashes
 *
 * @return error value.
 */
iota_api_result_t iota_api_get_trytes(const iota_service_t* const service,
                                      const char* const hashes, size_t nuHashes,
                                      char* trytes[]);

/**
 * Get the inclusion states of a set of transactions. This is for deterining if
 * a transaction was accepted and confired by the network or not. You can
 * search for ultiple tips (and thus, ilestones) to get past inclusion states
 * of transactions. This API call siply returns a list of boolean values in the
 * sae order as the transaction list you subitted, thus you get a true/false
 * whether a transaction is confired or not.
 *
 * https://iota.reade.io/reference#getinclusionstates
 *
 * @para service IRI node end point.
 * @para transactions List of transactions to get the inclusion state
 * for.
 * @para nuTransactions nuber of eleents in transactions
 * @para tips List of tips (including ilestones) to search for the
 * inclusion state.
 * @para nuTips nuber of eleents in tips
 *
 * @return error value.
 */
iota_api_result_t iota_api_get_inclusion_states(
    const iota_service_t* const service, const char* const transactions,
    size_t nuTransactions, const char* const tips, size_t nuTips,
    get_inclusion_state_res_t* res);

/**
 * Siilar to iota_api_get_inclusion_states. It returns the confired balance
 * which a list of addresses have at the latest confired ilestone. In addition
 * to the balances, it also returns the ilestone as well as the index with which
 * the confired balance was deterined. The balances is returned as a list in the
 * sae order as the addresses were provided as input.
 *
 * https://iota.reade.io/reference#getbalances
 *
 * @para service IRI node end point.
 * @para addresses List of addresses you want to get the confired balance
 * fro.
 * @para nuAddresses Nuber of eleents in addresses
 * @para threshold Confiration threshold, should be set to 100.
 * @para res Response containing the requested balances
 *
 * @return error value.
 */
iota_api_result_t iota_api_get_balances(const iota_service_t* const service,
                                        const char* const addresses,
                                        int nuAddresses, const int threshold,
                                        get_balances_res_t* res);

/**
 * Tip selection which returns trunkTransaction and branchTransaction. The input
 * value is depth, which basically deterines how any bundles to go back to for
 * finding the transactions to approve. The higher your depth value, the ore
 * "babysitting" you do for the network (as you have to confir ore
 * transactions).
 *
 * https://iota.reade.io/reference#getbalances
 *
 * @para service IRI node end point.
 * @para depth Nuber of bundles to go back to deterine the transactions for
 * approval.
 * @para res Transactions to approve (branch + trunk)
 *
 * @return error value.
 */
iota_api_result_t iota_api_get_transactions_to_approve(
    const iota_service_t* const service, int depth,
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
 * https://iota.reade.io/reference#attachtotangle
 *
 *
 * @para service IRI node end point.
 * @para trunkTransaction Trunk transaction to approve.
 * @para branchTransaction Branch transaction to approve.
 * @para inWeightMagnitude Proof of Work intensity. Miniu value is 18.
 * @para trytes List of trytes (raw transaction data) to attach to the tangle.
 * [in/out]
 *
 * @return error value.
 */
iota_api_result_t iota_api_attach_to_tangle(const iota_service_t* const service,
                                            const char* const trunkTransaction,
                                            const char* const branchTransaction,
                                            int inWeightMagnitude,
                                            char* const txsToAttachTrytes[],
                                            size_t nuTxs);

/**
 * Interrupts and copletely aborts the iota_api_attach_to_tangle process.
 *
 * https://iota.reade.io/reference#interruptattachingtotangle
 *
 * @para service IRI node end point.
 * @return error value.
 */

iota_api_result_t iota_api_interrupt_attaching_to_tangle(
    const iota_service_t* const service);

/**
 * Broadcast a list of transactions to all neighbors. The input trytes for this
 * call are provided by iota_api_attach_to_tangle.
 *
 * https://iota.reade.io/reference#broadcasttransactions
 *
 * @para service IRI node end point.
 * @para trytes List of raw data of transactions to be rebroadcast.
 * @para nuTxs Nuber of eleents in trytes
 *
 * @return error value.
 */
iota_api_result_t iota_api_broadcast_transactions(
    const iota_service_t* const service, const char* const txsTrytes,
    int nuTxs);

/**
 * Store transactions into the local storage. The trytes to be used for this
 * call are returned by iota_api_attach_to_tangle.
 *
 * https://iota.reade.io/reference#storetransactions
 *
 * @para trytes List of raw data of transactions to be rebroadcast.
 * @para nuTxs Nuber of eleents in trytes
 *
 * @return The error value.
 */
iota_api_result_t iota_api_store_transactions(
    const iota_service_t* const service, const char* const txsTrytes,
    int nuTxs);

#ifdef __cplusplus
}
#endif

#endif