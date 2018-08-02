/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#ifdef __cplusplus
extern "C" {
#endif

#ifndef __COMMON_MODEL_TRANSACTION_H_
#define __COMMON_MODEL_TRANSACTION_H_

#include "common/trinary/trit_array.h"

#define NUM_TRITS_SERIALIZED_TRANSACTION 8019
#define NUM_TRITS_SIGNATURE 6561
#define NUM_TRITS_ADDRESS 243
#define NUM_TRITS_VALUE 81
#define NUM_TRITS_OBSOLETE_TAG 81
#define NUM_TRITS_TIMESTAMP 27
#define NUM_TRITS_CURRENT_INDEX 27
#define NUM_TRITS_LAST_INDEX 27
#define NUM_TRITS_BUNDLE 243
#define NUM_TRITS_TRUNK 243
#define NUM_TRITS_BRANCH 243
#define NUM_TRITS_TAG 81
#define NUM_TRITS_ATTACHMENT_TIMESTAMP 27
#define NUM_TRITS_ATTACHMENT_TIMESTAMP_LOWER 27
#define NUM_TRITS_ATTACHMENT_TIMESTAMP_UPPER 27
#define NUM_TRITS_NOUNCE 81

#define NUM_TRYTES_SERIALIZED_TRANSACTION 2673
#define NUM_TRYTES_SIGNATURE 2187
#define NUM_TRYTES_ADDRESS 81
#define NUM_TRYTES_VALUE 27
#define NUM_TRYTES_OBSOLETE_TAG 27
#define NUM_TRYTES_TIMESTAMP 9
#define NUM_TRYTES_CURRENT_INDEX 9
#define NUM_TRYTES_LAST_INDEX 9
#define NUM_TRYTES_BUNDLE 81
#define NUM_TRYTES_TRUNK 81
#define NUM_TRYTES_BRANCH 81
#define NUM_TRYTES_TAG 27
#define NUM_TRYTES_ATTACHMENT_TIMESTAMP 9
#define NUM_TRYTES_ATTACHMENT_TIMESTAMP_LOWER 9
#define NUM_TRYTES_ATTACHMENT_TIMESTAMP_UPPER 9
#define NUM_TRYTES_NOUNCE 27

#if defined(TRIT_ARRAY_ENCODING_1_TRIT_PER_BYTE)
#define FLEX_TRIT_SIZE_27 27
#define FLEX_TRIT_SIZE_81 81
#define FLEX_TRIT_SIZE_243 243
#define FLEX_TRIT_SIZE_6561 6561
#elif defined(TRIT_ARRAY_ENCODING_3_TRITS_PER_BYTE)
#define FLEX_TRIT_SIZE_27 9
#define FLEX_TRIT_SIZE_81 27
#define FLEX_TRIT_SIZE_243 81
#define FLEX_TRIT_SIZE_6561 2187
#elif defined(TRIT_ARRAY_ENCODING_4_TRITS_PER_BYTE)
#define FLEX_TRIT_SIZE_27 7
#define FLEX_TRIT_SIZE_81 21
#define FLEX_TRIT_SIZE_243 61
#define FLEX_TRIT_SIZE_6561 1641
#elif defined(TRIT_ARRAY_ENCODING_5_TRITS_PER_BYTE)
#define FLEX_TRIT_SIZE_27 6
#define FLEX_TRIT_SIZE_81 17
#define FLEX_TRIT_SIZE_243 49
#define FLEX_TRIT_SIZE_6561 1313
#endif

/***********************************************************************************************************
 * Transaction data structure
 ***********************************************************************************************************/
typedef struct _iota_transaction *iota_transaction_t;
struct _iota_transaction {
  // 2187 trytes = 6561 trits
  flex_trit_t signature_or_message[FLEX_TRIT_SIZE_6561];
  // 81 trytes = 243 trits
  flex_trit_t address[FLEX_TRIT_SIZE_243];
  // 27 trytes = 81 trits
  int64_t value;
  // 27 trytes = 81 trits
  flex_trit_t obsolete_tag[FLEX_TRIT_SIZE_81];
  // 9 trytes = 27 trits
  uint64_t timestamp;
  // 9 trytes = 27 trits
  int64_t current_index;
  // 9 trytes = 27 trits
  int64_t last_index;
  // 81 trytes = 243 trits
  flex_trit_t bundle[FLEX_TRIT_SIZE_243];
  // 81 trytes = 243 trits
  flex_trit_t trunk[FLEX_TRIT_SIZE_243];
  // 81 trytes = 243 trits
  flex_trit_t branch[FLEX_TRIT_SIZE_243];
  // 27 trytes = 81 trits
  flex_trit_t tag[FLEX_TRIT_SIZE_81];
  // 9 trytes = 27 trits
  int64_t attachment_timestamp;
  // 9 trytes = 27 trits
  int64_t attachment_timestamp_lower;
  // 9 trytes = 27 trits
  int64_t attachment_timestamp_upper;
  // 27 trytes = 81 trits
  flex_trit_t nonce[FLEX_TRIT_SIZE_81];
  // Total 2673 trytes
};

/***********************************************************************************************************
 * Accessors
 ***********************************************************************************************************/
// Get the transaction signature
flex_trit_t *transaction_signature(iota_transaction_t transaction);
// Set the transaction signature (copy argument)
void transaction_set_signature(iota_transaction_t transaction,
                               const flex_trit_t *signature);
// Get the transaction message
flex_trit_t *transaction_message(iota_transaction_t transaction);
// Set the transaction message (copy argument)
void transaction_set_message(iota_transaction_t transaction,
                             const flex_trit_t *message);
// Get the transaction address
flex_trit_t *transaction_address(iota_transaction_t transaction);
// Set the transaction address (copy argument)
void transaction_set_address(iota_transaction_t transaction,
                             const flex_trit_t *address);
// Get the transaction value
int64_t transaction_value(iota_transaction_t transaction);
// Set the transaction value
void transaction_set_value(iota_transaction_t transaction, int64_t value);
// Get the transaction obsolete tag
flex_trit_t *transaction_obsolete_tag(iota_transaction_t transaction);
// Set the transaction obsolete tag
void transaction_set_obsolete_tag(iota_transaction_t transaction,
                                  const flex_trit_t *obsolete_tag);
// Get the transaction timestamp
uint64_t transaction_timestamp(iota_transaction_t transaction);
// Set the transaction timestamp
void transaction_set_timestamp(iota_transaction_t transaction,
                               uint64_t timestamp);
// Get the transaction current index
int64_t transaction_current_index(iota_transaction_t transaction);
// Set the transaction current index
void transaction_set_current_index(iota_transaction_t transaction,
                                   int64_t index);
// Get the transaction last index
int64_t transaction_last_index(iota_transaction_t transaction);
// Set the transaction last index
void transaction_set_last_index(iota_transaction_t transaction,
                                int64_t last_index);
// Get the transaction bundle
flex_trit_t *transaction_bundle(iota_transaction_t transaction);
// Set the transaction bundle (copy argument)
void transaction_set_bundle(iota_transaction_t transaction,
                            const flex_trit_t *bundle);
// Get the transaction trunk
flex_trit_t *transaction_trunk(iota_transaction_t transaction);
// Set the transaction trunk (copy argument)
void transaction_set_trunk(iota_transaction_t transaction,
                           const flex_trit_t *trunk);
// Get the transaction branch
flex_trit_t *transaction_branch(iota_transaction_t transaction);
// Set the transaction branch (copy argument)
void transaction_set_branch(iota_transaction_t transaction,
                            const flex_trit_t *branch);
// Get the transaction tag
flex_trit_t *transaction_tag(iota_transaction_t transaction);
// Set the transaction tag (copy argument)
void transaction_set_tag(iota_transaction_t transaction,
                         const flex_trit_t *tag);
// Get the transaction attachement timestamp
int64_t transaction_attachment_timestamp(iota_transaction_t transaction);
// Set the transaction attachement timestamp
void transaction_set_attachment_timestamp(iota_transaction_t transaction,
                                          int64_t timestamp);
// Get the transaction attachement timestamp lower
int64_t transaction_attachment_timestamp_lower(iota_transaction_t transaction);
// Set the transaction attachement timestamp lower
void transaction_set_attachment_timestamp_lower(iota_transaction_t transaction,
                                                int64_t timestamp);
// Get the transaction attachement timestamp upper
int64_t transaction_attachment_timestamp_upper(iota_transaction_t transaction);
// Set the transaction attachement timestamp upper
void transaction_set_attachment_timestamp_upper(iota_transaction_t transaction,
                                                int64_t timestamp);
// Get the transaction nonce
flex_trit_t *transaction_nonce(iota_transaction_t transaction);
// Set the transaction nonce (copy argument)
void transaction_set_nonce(iota_transaction_t transaction,
                           const flex_trit_t *nonce);

/***********************************************************************************************************
 * Constructors
 ***********************************************************************************************************/
// Creates and returns a new transaction without data
iota_transaction_t transaction_new(void);
// Creates and returns a new transaction from serialized data - NULL if failed
iota_transaction_t transaction_deserialize(const flex_trit_t *trits);
/***********************************************************************************************************
 * Serialization
 ***********************************************************************************************************/
// Returns the serialized data from an existing transaction - NULL if failed
flex_trit_t *transaction_serialize(const iota_transaction_t transaction);
// Places the serialized data from an existing transaction in pre-allocated
// string - returns non 0 on success
size_t transaction_serialize_on_flex_trits(const iota_transaction_t transaction,
                                           flex_trit_t *trits);
/***********************************************************************************************************
 * Deserialization
 ***********************************************************************************************************/
// Fills up an existing transaction with the serialized data in trits - returns
// non 0 on success
size_t transaction_deserialize_from_trits(iota_transaction_t transaction,
                                          const flex_trit_t *trits);
/***********************************************************************************************************
 * Destructor
 ***********************************************************************************************************/
// Free an existing transaction - compatible with free()
void transaction_free(void *transaction);

#endif  // __COMMON_MODEL_TRANSACTION_H_
#ifdef __cplusplus
}
#endif
