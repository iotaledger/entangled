/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://gitlab.com/iota-foundation/software/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#ifdef __cplusplus
extern "C" {
#endif

#ifndef __COMMON_MODEL_TRANSACTION_H_
#define __COMMON_MODEL_TRANSACTION_H_

#include "common/trinary/tryte.h"

/***********************************************************************************************************
 * Transaction data structure
 ***********************************************************************************************************/
typedef struct _iota_transaction *iota_transaction;
struct _iota_transaction {
    // 2187 trytes
    tryte_t  signature_or_message[2187];
    // 81 trytes
    tryte_t  address[81];
    // 27 trytes
    int64_t  value;
    // 27 trytes
    uint64_t obsolete_tag;
    // 9 trytes
    uint64_t timestamp;
    // 9 trytes
    int64_t  current_index;
    // 9 trytes
    int64_t  last_index;
    // 81 trytes
    tryte_t  bundle[81];
    // 81 trytes
    tryte_t  trunk[81];
    // 81 trytes
    tryte_t  branch[81];
    // 27 trytes
    tryte_t  tag[27];
    // 9 trytes
    int64_t  attachment_timestamp;
    // 9 trytes
    int64_t  attachment_timestamp_lower;
    // 9 trytes
    int64_t  attachment_timestamp_upper;
    // 27 trytes
    tryte_t  nonce[27];
    // Total 2673 trytes
};

/***********************************************************************************************************
 * Accessors
 ***********************************************************************************************************/
// Get the transaction signature
tryte_t *transaction_signature(iota_transaction transaction);
// Set the transaction signature (copy argument)
void transaction_set_signature(iota_transaction transaction, const tryte_t *signature);
// Get the transaction message
tryte_t *transaction_message(iota_transaction transaction);
// Set the transaction message (copy argument)
void transaction_set_message(iota_transaction transaction, const tryte_t *message);
// Get the transaction address
tryte_t *transaction_address(iota_transaction transaction);
// Set the transaction address (copy argument)
void transaction_set_address(iota_transaction transaction, const tryte_t *address);
// Get the transaction value
int64_t transaction_value(iota_transaction transaction);
// Set the transaction value
void transaction_set_value(iota_transaction transaction, int64_t value);
// Get the transaction obsolete tag
uint64_t transaction_obsolete_tag(iota_transaction transaction);
// Set the transaction obsolete tag
void transaction_set_obsolete_tag(iota_transaction transaction, uint64_t obsolete_tag);
// Get the transaction timestamp
uint64_t transaction_timestamp(iota_transaction transaction);
// Set the transaction timestamp
void transaction_set_timestamp(iota_transaction transaction, uint64_t timestamp);
// Get the transaction current index
int64_t transaction_current_index(iota_transaction transaction);
// Set the transaction current index
void transaction_set_current_index(iota_transaction transaction, int64_t index);
// Get the transaction last index
int64_t transaction_last_index(iota_transaction transaction);
// Set the transaction last index
void transaction_set_last_index(iota_transaction transaction, int64_t last_index);
// Get the transaction bundle
tryte_t *transaction_bundle(iota_transaction transaction);
// Set the transaction bundle (copy argument)
void transaction_set_bundle(iota_transaction transaction, const tryte_t *bundle);
// Get the transaction trunk
tryte_t *transaction_trunk(iota_transaction transaction);
// Set the transaction trunk (copy argument)
void transaction_set_trunk(iota_transaction transaction, const tryte_t *trunk);
// Get the transaction branch
tryte_t *transaction_branch(iota_transaction transaction);
// Set the transaction branch (copy argument)
void transaction_set_branch(iota_transaction transaction, const tryte_t *branch);
// Get the transaction tag
tryte_t *transaction_tag(iota_transaction transaction);
// Set the transaction tag (copy argument)
void transaction_set_tag(iota_transaction transaction, const tryte_t *tag);
// Get the transaction attachement timestamp
int64_t transaction_attachment_timestamp(iota_transaction transaction);
// Set the transaction attachement timestamp
void transaction_set_attachment_timestamp(iota_transaction transaction, int64_t timestamp);
// Get the transaction attachement timestamp lower
int64_t transaction_attachment_timestamp_lower(iota_transaction transaction);
// Set the transaction attachement timestamp lower
void transaction_set_attachment_timestamp_lower(iota_transaction transaction, int64_t timestamp);
// Get the transaction attachement timestamp upper
int64_t transaction_attachment_timestamp_upper(iota_transaction transaction);
// Set the transaction attachement timestamp upper
void transaction_set_attachment_timestamp_upper(iota_transaction transaction, int64_t timestamp);
// Get the transaction nonce
tryte_t *transaction_nonce(iota_transaction transaction);
// Set the transaction nonce (copy argument)
void transaction_set_nonce(iota_transaction transaction, const tryte_t *nonce);

/***********************************************************************************************************
 * Constructors
 ***********************************************************************************************************/
// Creates and returns a new transaction without data
iota_transaction transaction_new(void);
// Creates and returns a new transaction from serialized data - NULL if failed
iota_transaction transaction_deserialize(const tryte_t *trytes);
/***********************************************************************************************************
 * Serialization
 ***********************************************************************************************************/
// Returns the serialized data from an existing transaction - NULL if failed
tryte_t *transaction_serialize(const iota_transaction transaction);
// Places the serialized data from an existing transaction in pre-allocated string - returns non 0 on success
uint16_t transaction_serialize_on_trytes(const iota_transaction transaction, tryte_t *trytes);
/***********************************************************************************************************
 * Deserialization
 ***********************************************************************************************************/
// Fills up an existing transaction with the serialized data in trytes - returns non 0 on success
uint16_t transaction_deserialize_from_trytes(iota_transaction transaction, const tryte_t *trytes);
/***********************************************************************************************************
 * Destructor
 ***********************************************************************************************************/
// Free an existing transaction - compatible with free()
void transaction_free(void *transaction);

#endif // __COMMON_MODEL_TRANSACTION_H_
#ifdef __cplusplus
}
#endif

