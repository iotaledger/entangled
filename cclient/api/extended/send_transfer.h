/*
 * Copyright (c) 2019 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#ifndef CCLIENT_API_SEND_TRANSFER_H
#define CCLIENT_API_SEND_TRANSFER_H

#include "cclient/http/http.h"
#include "common/model/bundle.h"
#include "common/model/transfer.h"

#ifdef __cplusplus
extern "C" {
#endif

// https://github.com/iotaledger/iota.js/blob/next/packages/core/src/createSendTransfer.ts#L22
retcode_t iota_client_send_transfer(iota_client_service_t const* const serv, int const depth, int const mwm,
                                    bool local_pow, transfer_t** const transfers, uint32_t num_transfer,
                                    flex_trit_t const* const reference, bundle_transactions_t* out_tx_objs);

#ifdef __cplusplus
}
#endif

#endif  // CCLIENT_API_SEND_TRANSFER_H