/*
 * Copyright (c) 2019 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#ifndef CCLIENT_API_ATTACH_TO_TANGLE_H
#define CCLIENT_API_ATTACH_TO_TANGLE_H

#include "cclient/http/http.h"
#include "cclient/request/attach_to_tangle.h"
#include "cclient/response/attach_to_tangle.h"

#ifdef __cplusplus
extern "C" {
#endif

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
 * @param service IRI node end point, NULL for local PoW.
 * @param req Request containing the attachment params
 * @param res Response containing trytes
 *
 * @return error value.
 */
retcode_t iota_client_attach_to_tangle(const iota_client_service_t* const service,
                                       const attach_to_tangle_req_t* const req, attach_to_tangle_res_t* res);

#ifdef __cplusplus
}
#endif

#endif  // CCLIENT_API_ATTACH_TO_TANGLE_H
