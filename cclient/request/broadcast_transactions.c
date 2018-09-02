#include "broadcast_transactions.h"

broadcast_transactions_req_t* broadcast_transactions_req_new() {
  broadcast_transactions_req_t* transactions = NULL;
  utarray_new(transactions, &ut_str_icd);
  return transactions;
}

void broadcast_transactions_req_add(broadcast_transactions_req_t* transactions,
                                    const char* trytes) {
  utarray_push_back(transactions, &trytes);
}

void broadcast_transactions_req_free(
    broadcast_transactions_req_t* transactions) {
  utarray_free(transactions);
}
