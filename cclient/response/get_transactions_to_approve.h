// Copyright 2018 IOTA Foundation

#ifndef CCLIENT_RESPONSE_GET_TRANSACTIONS_TO_APPROVE_H
#define CCLIENT_RESPONSE_GET_TRANSACTIONS_TO_APPROVE_H

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
  flex_trit_p branchTransaction;
  flex_trit_p trunkTransaction;

} get_transactions_to_approve_res_t;

#ifdef __cplusplus
}
#endif

#endif  // CCLIENT_RESPONSE_GET_TRANSACTIONS_TO_APPROVE_H
