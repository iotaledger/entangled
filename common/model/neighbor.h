#ifndef __COMMON_MODEL_NEIGHBOR_H__
#define __COMMON_MODEL_NEIGHBOR_H__

typedef struct {
  char *address;
  unsigned int number_of_all_transactions;
  unsigned int number_of_new_transactions;
  unsigned int number_of_invalid_transactions;
  unsigned int random_transaction_requests;
  unsigned int number_of_sent_transactions;
} neighbor_t;

#endif
