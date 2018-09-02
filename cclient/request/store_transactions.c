#include "store_transactions.h"

store_transactions_req_t* store_transactions_req_new() {
  store_transactions_req_t* transactions = NULL;
  utarray_new(transactions, &ut_str_icd);
  return transactions;
}

void store_transactions_req_add(store_transactions_req_t* transactions,
                                const char* trytes) {
  utarray_push_back(transactions, &trytes);
}

void store_transactions_req_free(store_transactions_req_t* transactions) {
  utarray_free(transactions);
}
