/*
 * Copyright (c) 2019 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#ifndef CCLIENT_API_PREPARE_TRANSFERS_H
#define CCLIENT_API_PREPARE_TRANSFERS_H

#include "cclient/http/http.h"
#include "common/model/bundle.h"
#include "common/model/transfer.h"

#ifdef __cplusplus
extern "C" {
#endif

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
retcode_t iota_client_prepare_transfers(iota_client_service_t const* const serv, transfer_t** const transfers,
                                        uint32_t const num_transfer, bundle_transactions_t* out_bundle);

#ifdef __cplusplus
}
#endif

#endif  // CCLIENT_API_PREPARE_TRANSFERS_H