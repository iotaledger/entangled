/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#ifndef CCLIENT_IOTA_EXTENDED_API_H_
#define CCLIENT_IOTA_EXTENDED_API_H_

#include "common/helpers/sign.h"
#include "iota_client_core_api.h"
#include "utils/containers/hash/hash243_queue.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef flex_hash_array_t hashes_t;
typedef flex_hash_array_t address_list_t;
typedef flex_hash_array_t inputs_list_t;
typedef flex_hash_array_t transfer_list_t;
typedef flex_hash_array_t transaction_list_t;
typedef flex_hash_array_t transaction_objs_t;
typedef flex_hash_array_t booleans_t;
typedef flex_hash_array_t input_list_t;

typedef struct {
  int64_t balance;
  int32_t key_index;
  int security;
  trit_array_p address;
} input_t;

typedef struct {
  uint64_t total_balance;
  hash243_queue_t addresses;
} inputs_t;

typedef struct {
  size_t balance;
  flex_trit_t latest_address[FLEX_TRIT_SIZE_243];
  hash243_queue_t addresses;
  hash243_queue_t transactions;
} account_data_t;

typedef struct {
  size_t security;
  size_t start;
  size_t total;
} address_opt_t;

void iota_client_extended_init();
void iota_client_extended_destroy();

/**
 * Re-broadcasts all transactions in a bundle given the tail transaction hash.
 * It might be useful when transactions did not properly propagate,
 * particularly in the case of large bundles.
 *
 * @param {iota_client_service_t} serv - client service
 * @param {trit_array_p} tail_hash - Tail transaction hash
 * @param {transaction_objs_t} out_tx_objs - List of transaction objects
 *
 * @returns {retcode_t}
 * - `INVALID_HASH`: Invalid tail transaction hash
 * - `INVALID_BUNDLE`: Invalid bundle
 *
 * Refer:
 * https://github.com/iotaledger/iota.js/blob/next/packages/core/src/createBroadcastBundle.ts#L38
 */
retcode_t iota_client_broadcast_bundle(iota_client_service_t const* const serv,
                                       trit_array_p const* const tail_hash,
                                       transaction_list_t* out_tx_objs);

/**
 * Wrapper function for iota_client_find_transactions() and
 * iota_client_get_trytes(). Searches for transactions given a `query` object
 * with `addresses`, `tags` and `approvees` fields. Multiple query fields are
 * supported and `iota_client_find_transaction_objects()` returns intersection
 * of results.
 *
 * @param {iota_client_service_t} serv - client service
 * @param {hashes_t} addresses - List of addresses
 * @param {hashes_t} bundles - List of bundle hashes
 * @param {hashes_t} tags - List of tags
 * @param {hashes_t} approvees - List of approvees
 * @param {transaction_list_t} out_tx_objs - List ofatransaction objects.
 *
 * @returns {retcode_t}
 * - `INVALID_SEARCH_KEY`
 * - `INVALID_HASH`: Invalid bundle hash
 * - `INVALID_TRANSACTION_HASH`: Invalid approvee transaction hash
 * - `INVALID_ADDRESS`: Invalid address
 * - `INVALID_TAG`: Invalid tag
 *
 * Refer:
 * https://github.com/iotaledger/iota.js/blob/next/packages/core/src/createFindTransactionObjects.ts#L38
 */
retcode_t iota_client_find_transaction_objects(
    iota_client_service_t const* const serv, hashes_t* addresses,
    hashes_t const* const bundles, hashes_t const* const tags,
    hashes_t const* const approvees, transaction_list_t* out_tx_objs);

/**
 * Returns an `account_data_t` object, containing account information about
 * `addresses`, `transactions` and the total balance.
 *
 * @param {iota_client_service_t} serv - client service
 * @param {flex_trit_t} seed
 * @param {size_t} security - Security
 * @param {account_data_t} out_account - acount data object
 *
 * @returns {retcode_t}
 * - `INVALID_SEED`
 * - `INVALID_START_OPTION`
 * - `INVALID_START_END_OPTIONS`: Invalid combination of start & end options`
 *
 * Refer:
 * https://github.com/iotaledger/iota.js/blob/next/packages/core/src/createGetAccountData.ts#L84
 */
retcode_t iota_client_get_account_data(iota_client_service_t const* const serv,
                                       flex_trit_t const* const seed,
                                       size_t const security,
                                       account_data_t* out_account);

/**
 * Fetches and validates the bundle given a _tail_ transaction hash, by calling
 * iota_client_traverse_bundle() and traversing through `trunk transaction`.
 *
 * @param {iota_client_service_t} serv - client service
 * @param {trit_array_p} tail_hash- Tail transaction hash
 * @param {transaction_objs_t} out_tx_objs - Bundle as array of transaction
 * objects.
 *
 * @returns {retcode_t}
 * - `INVALID_TRANSACTION_HASH`
 * - `INVALID_TAIL_HASH`: Provided transaction is not tail (`currentIndex !==
 * 0`)
 * - `INVALID_BUNDLE`: Bundle is syntactically invalid
 * - Fetch error
 *
 * https://github.com/iotaledger/iota.js/blob/next/packages/core/src/createGetBundle.ts#L38
 */
retcode_t iota_client_get_bundle(iota_client_service_t const* const serv,
                                 trit_array_p const tail_hash,
                                 transaction_objs_t* out_tx_objs);

/**
 * Creates and returns an `Inputs` object by generating addresses and fetching
 * their latest balance.
 *
 * @param {iota_client_service_t} serv - client service
 * @param {trit_array_p} seed
 * @param {address_opt_t} addr_opt - address options: Starting key index,
 * Security level, Ending Key index.
 * @param {threshold} [options.threshold] - Minimum amount of balance required
 * @param {inputs_t} out_input - Inputs object containg a list of `Address`
 * objects and `totalBalance` field.
 *
 * @returns {retcode_t}
 * - `INVALID_SEED`
 * - `INVALID_SECURITY_LEVEL`
 * - `INVALID_START_OPTION`
 * - `INVALID_START_END_OPTIONS`
 * - `INVALID_THRESHOLD`
 * - `INSUFFICIENT_BALANCE`
 * - Fetch error
 *
 * https://github.com/iotaledger/iota.js/blob/next/packages/core/src/createGetInputs.ts#L70
 */
retcode_t iota_client_get_inputs(iota_client_service_t const* const serv,
                                 flex_trit_t const* const seed,
                                 address_opt_t const addr_opt,
                                 uint64_t const threshold,
                                 inputs_t* const out_input);

/**
 * Fetches inclusion states of given transactions and a list of tips,
 * by calling iota_client_get_inclusion_status` on
 * `latest_solid_subtangle_milestione`.
 *
 * @param {iota_client_service_t} serv - client service
 * @param {hashes_t} transactions - List of transactions hashes
 * @param {booleans_t} out_states - List of inclusion states
 *
 * @returns {retcode_t}
 * - `INVALID_HASH`: Invalid transaction hash
 * - Fetch error
 *
 * https://github.com/iotaledger/iota.js/blob/next/packages/core/src/createGetLatestInclusion.ts#L34
 */
retcode_t iota_client_get_latest_inclusion(
    iota_client_service_t const* const serv, hashes_t const* const transactions,
    booleans_t out_states);

/**
 * Generates and returns a new address by calling
 * iota_client_find_transactions() until the first unused address is detected.
 * This stops working after a snapshot.
 *
 * @param {iota_client_service_t} serv - client service
 * @param {trit_array_p} seed - At least 81 trytes long seed
 * @param {address_opt_t} addr_opt - address options: Starting key index,
 * Security level, Ending Key index.
 * @param {hash243_queue_t} out_addresses - New (unused) address or list of
 * addresses up to (and including) first unused address.
 *
 * @returns {retcode_t}
 * - `INVALID_SEED`
 * - `INVALID_SECURITY`
 *,
 *   https://github.com/iotaledger/iota.js/blob/next/packages/core/src/createGetNewAddress.ts#L131
 */
retcode_t iota_client_get_new_address(iota_client_service_t const* const serv,
                                      flex_trit_t const* const seed,
                                      address_opt_t const addr_opt,
                                      hash243_queue_t* out_addresses);

/**
 * Fetches the transaction objects, given an array of transaction hashes.
 *
 * @param {iota_client_service_t} serv - client service
 * @param {hashes_t} hashes - Array of transaction hashes
 * @param {transaction_objs_t} out_tx_objs - List of transaction objects.
 *
 * @returns {retcode_t}
 * - `INVALID_TRANSACTION_HASH`
 * - Fetch error
 *
 * https://github.com/iotaledger/iota.js/blob/next/packages/core/src/createGetTransactionObjects.ts#L35
 */
retcode_t iota_client_get_transaction_objects(
    iota_client_service_t const* const serv, hashes_t const* const transactions,
    transaction_objs_t* out_tx_objs);

/**
 * Checks if a transaction is _promotable_, by calling
 * iota_client_check_consistency() and verifying that
 * `attachmentTimestamp` is above a lower bound. Lower bound is calculated based
 * on number of milestones issued since transaction attachment.
 *
 * Using iota_client_is_promotable() to determine if transaction can be
 * [_promoted_] or should be [_reattached_]
 *
 * @param {iota_client_service_t} serv - client service
 * @param {trit_array_p} tail - Tail transaction hash
 * @param {boolean} out_promotable - boolean value
 *
 * @return {retcode_t}
 * - `INVALID_HASH`: Invalid hash
 * - `INVALID_DEPTH`: Invalid depth
 * - Fetch error
 *
 * https://github.com/iotaledger/iota.js/blob/next/packages/core/src/createIsPromotable.ts#L71
 */
retcode_t iota_client_is_promotable(iota_client_service_t const* const serv,
                                    trit_array_p const tail_transaction,
                                    bool* out_promotable);

/**
 * Prepares the transaction trytes by generating a bundle, filling in transfers
 * and inputs, adding remainder and signing. It can be used to generate and sign
 * bundles either online or offline.
 *
 * @param {iota_client_service_t} serv - client service
 * @param {trit_array_p} seed
 * @param {transfer_list_t} transfers
 * @param {input_list_t} inputs - Inputs used for signing. Needs to have correct
 * security, keyIndex and address value
 * @param {trit_array_p} remainder - Remainder address
 * @param {int} security - Security level to be used for getting inputs and
 * reminder address
 * @param {hashes_t} out_bundle_trytes - Returns bundle trytes
 *
 * @return {retcode_t}
 * - `INVALID_SEED`
 * - `INVALID_TRANSFER_ARRAY`
 * - `INVALID_INPUT`
 * - `INVALID_REMAINDER_ADDRESS`
 * - `INSUFFICIENT_BALANCE`
 * - `NO_INPUTS`
 * - `SENDING_BACK_TO_INPUTS`
 * - Fetch error, if connected to network
 *
 * https://github.com/iotaledger/iota.js/blob/next/packages/core/src/createPrepareTransfers.ts#L97
 */
retcode_t iota_client_prepare_transfers(iota_client_service_t const* const serv,
                                        trit_array_p const seed,
                                        int const security,
                                        transfer_list_t const* const transfers,
                                        trit_array_p const remainder,
                                        input_list_t const* const inputs,
                                        hashes_t* out_bundle_trytes);

/**
 * Attempts to promote a transaction using a provided bundle and, if successful,
 * returns the promoting Transactions.
 *
 * @param {iota_client_service_t} serv - client service
 * @param {trit_array_p} tail_hash
 * @param {int} depth
 * @param {int} minWeightMagnitude
 * @param {hashes_t} transfers
 *
 * @returns {retcode_t}
 * - `INCONSISTENT SUBTANGLE`: In this case promotion has no effect and
 * reatchment is required.
 * - Fetch error
 *
 * https://github.com/iotaledger/iota.js/blob/next/packages/core/src/createPromoteTransaction.ts#L51
 */
retcode_t iota_client_promote_transaction(
    iota_client_service_t const* const serv, trit_array_p const tail_hash,
    int const depth, int const mwm, transfer_list_t const* const transfers,
    transaction_list_t* out_transactions);

/**
 * Reattaches a transfer to tangle by selecting tips & performing the
 * Proof-of-Work again. Reattachments are usefull in case original transactions
 * are pending, and can be done securely as many times as needed.
 *
 * @param {iota_client_service_t} serv - client service
 * @param {trit_array_p} tail - Tail transaction hash. Tail transaction is the
 * transaction in the bundle with `currentIndex == 0`.
 * @param {int} depth - The depth at which Random Walk starts. A value of `3`
 * is typically used by wallets, meaning that RW starts 3 milestones back.
 * @param {int} mwm- Minimum number of trailing zeros in
 * transaction hash. This is used by iota_client_attach_to_tangle() function to
 * search for a valid `nonce`. Currently is `14` on mainnet & spamnnet and `9`
 * on most other testnets.
 * @param {transaction_objs_t} out_tx_objs - Analyzed Transaction objects.
 *
 *
 * @returns {retcode_t}
 * - `INVALID_DEPTH`
 * - `INVALID_MIN_WEIGHT_MAGNITUDE`
 * - `INVALID_TRANSACTION_HASH`
 * - `INVALID_BUNDLE`
 * - Fetch error
 *
 * https://github.com/iotaledger/iota.js/blob/next/packages/core/src/createReplayBundle.ts#L38
 */
retcode_t iota_client_replay_bundle(iota_client_service_t const* const serv,
                                    trit_array_p const tail, int const depth,
                                    int const mwm,
                                    transaction_objs_t out_tx_objs);

// https://github.com/iotaledger/iota.js/blob/next/packages/core/src/createSendTransfer.ts#L22
retcode_t iota_client_send_transfer(iota_client_service_t const* const serv,
                                    trit_array_p const seed, int const depth,
                                    int const mwm,
                                    transfer_list_t const* const transfers,
                                    transaction_objs_t out_tx_objs);

/**
 * [Attaches to tanlge]{@link #module_core.attachToTangle}, [stores]{@link
 * #module_core.storeTransactions} and [broadcasts]{@link
 * #module_core.broadcastTransactions} a list of transaction trytes.
 *
 * @param {iota_client_service_t} serv - client service
 * @param {hashes_t} trytes - List of trytes to attach, store & broadcast
 * @param {int} depth - Depth
 * @param {int} mwm - Min weight magnitude
 * @param {trit_array_p} reference - Optional reference hash
 * @param {hashes_t} out_transactions - Returns list of attached transactions
 *
 * @return {retcode_t}
 * - Fetch error, if connected to network
 *
 * https://github.com/iotaledger/iota.js/blob/next/packages/core/src/createSendTrytes.ts#L38
 */
retcode_t iota_client_send_trytes(iota_client_service_t const* const serv,
                                  hashes_t const* const trytes, int const depth,
                                  int const mwm, trit_array_p const reference,
                                  transaction_list_t* out_transactions);

/**
 * Stores and broadcasts a list of _attached_ transaction trytes by calling
 * iota_client_store_transactions() and iota_client_broadcast_transactions().
 *
 * Note: Persist the transaction trytes in local storage **before** calling this
 * command, to ensure that reattachment is possible, until your bundle has been
 * included.
 *
 * Any transactions stored with this command will eventaully be erased, as a
 * result of a snapshot.
 *
 * @param {hashes_t} trytes - Attached transaction trytes
 * @param {hashes_t} out_trytes - Attached transaction trytes
 *
 * @return {retcode_t}
 * @fulfil {Trytes[]} Attached transaction trytes
 * - `INVALID_ATTACHED_TRYTES`: Invalid attached trytes
 * - Fetch error
 *
 * https://github.com/iotaledger/iota.js/blob/next/packages/core/src/createStoreAndBroadcast.ts#L28
 */
retcode_t iota_client_store_and_broadcast(
    iota_client_service_t const* const serv, hashes_t const* const trytes,
    hashes_t* out_trytes);

/**
 * Fetches the bundle of a given the _tail_ transaction hash, by traversing
 * through `trunkTransaction`. It does not validate the bundle.
 *
 * @param {trit_array_p} trunk_tx- Trunk transaction, should be tail
 * (`currentIndex == 0`)
 * @param {hashes_t} bundles - List of accumulated transactions
 * @param {transaction_objs_t} out_tx_objs - Bundle as array of transaction
 * objects
 *
 * @returns {retcode_t}
 * - `INVALID_TRANSACTION_HASH`
 * - `INVALID_TAIL_HASH`: Provided transaction is not tail (`currentIndex !==
 * 0`)
 * - `INVALID_BUNDLE`: Bundle is syntactically invalid
 * - Fetch error
 *
 * https://github.com/iotaledger/iota.js/blob/next/packages/core/src/createTraverseBundle.ts#L36
 */
retcode_t iota_client_traverse_bundle(iota_client_service_t const* const serv,
                                      trit_array_p const trunk_tx,
                                      hashes_t const* const bundles,
                                      transfer_list_t* out_tx_objs);

#ifdef __cplusplus
}
#endif

#endif  // CCLIENT_IOTA_EXTENDED_API_H_
