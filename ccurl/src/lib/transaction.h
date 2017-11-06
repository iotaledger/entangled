#ifndef __TRANSACTION_H__
#define __TRANSACTION_H__
#include "constants.h"

typedef struct __attribute__((packed)) {
  trit_t message[MESSAGE_LENGTH];
  trit_t address[HASH_LENGTH];
  trit_t value[VALUE_LENGTH];
  trit_t obsolete_tag[NONCE_LENGTH];
  trit_t timestamp[INT_LENGTH];
  trit_t index[INT_LENGTH];
  trit_t end[INT_LENGTH];
  trit_t bundle[HASH_LENGTH];
  trit_t trunk[HASH_LENGTH];
  trit_t branch[HASH_LENGTH];
  trit_t tag[NONCE_LENGTH];
  trit_t attachment_timestamp[INT_LENGTH];
  trit_t attachment_timestamp_lower_bound[INT_LENGTH];
  trit_t attachment_timestamp_upper_bound[INT_LENGTH];
  trit_t nonce[NONCE_LENGTH];
} transaction_t;
#endif // __TRANSACTION_H__

