// Copyright 2018 IOTA Foundation

#include "iota_api.h"

iota_api_result_t iota_api_get_node_info(const iota_service_t* const service,
                                         get_node_info_res_t* const res) {
  iota_api_result_t result;
  return result;
}

iota_api_result_t iota_api_get_neighbors(const iota_service_t* const service,
                                         get_neighbors_res_t* const res) {
  iota_api_result_t result;
  return result;
}

iota_api_result_t iota_api_add_neighbors(const iota_service_t* const service,
                                         const char* const uris, int size) {
  iota_api_result_t result;
  return result;
}

iota_api_result_t iota_api_remove_neighbors(const iota_service_t* const service,
                                            const char* const uris, int size) {
  iota_api_result_t result;
  return result;
}

iota_api_result_t iota_api_get_tips(const iota_service_t* const service,
                                    get_tips_res_t* const res) {
  iota_api_result_t result;
  return result;
}

iota_api_result_t iota_api_find_transactions(
    const iota_service_t* const service,
    const find_transaction_req_t* const req,
    find_transactions_res_t* const res) {
  iota_api_result_t result;
  return result;
}

iota_api_result_t iota_api_get_trytes(const iota_service_t* const service,
                                      const char* const hashes, size_t size,
                                      char* trytes[]) {
  iota_api_result_t result;
  return result;
}

iota_api_result_t iota_api_get_inclusion_states(
    const iota_service_t* const service, const char* const transactions,
    size_t nuTransactions, const char* const tips, size_t nuTips,
    get_inclusion_state_res_t* res) {
  iota_api_result_t result;
  return result;
}

iota_api_result_t iota_api_get_balances(const iota_service_t* const service,
                                        const char* const addresses, int size,
                                        const int threshold,
                                        get_balances_res_t* res) {
  iota_api_result_t result;
  return result;
}

iota_api_result_t iota_api_get_transactions_to_approve(
    const iota_service_t* const service, int depth,
    get_transactions_to_approve_res_t* res) {
  iota_api_result_t result;
  return result;
}

iota_api_result_t iota_api_attach_to_tangle(const iota_service_t* const service,
                                            const char* const trunkTransaction,
                                            const char* const branchTransaction,
                                            int inWeightMagnitude,
                                            char* const txsToAttachTrytes[],
                                            size_t nuTxs) {
  iota_api_result_t result;
  return result;
}

iota_api_result_t iota_api_interrupt_attaching_to_tangle(
    const iota_service_t* const service) {
  iota_api_result_t result;
  return result;
}

iota_api_result_t iota_api_broadcast_transactions(
    const iota_service_t* const service, const char* const txsTrytes,
    int size) {
  iota_api_result_t result;
  return result;
}

iota_api_result_t iota_api_store_transactions(
    const iota_service_t* const service, const char* const txsTrytes,
    int size) {
  iota_api_result_t result;
  return result;
}