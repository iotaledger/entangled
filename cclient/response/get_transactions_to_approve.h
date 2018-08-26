/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#ifndef CCLIENT_RESPONSE_GET_TRANSACTIONS_TO_APPROVE_H
#define CCLIENT_RESPONSE_GET_TRANSACTIONS_TO_APPROVE_H

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
  address_t branchTransaction;
  address_t trunkTransaction;

} get_transactions_to_approve_res_t;

#ifdef __cplusplus
}
#endif

#endif  // CCLIENT_RESPONSE_GET_TRANSACTIONS_TO_APPROVE_H
