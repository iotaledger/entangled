/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://gitlab.com/iota-foundation/software/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#include "common/model/transaction.h"
#include <stdlib.h>
#include <string.h>
#include "common/trinary/trit_long.h"

/***********************************************************************************************************
 * Transaction
 ***********************************************************************************************************/
// To obfuscate the model, the structure definition comees here
// struct _iota_transaction_t {...}

/***********************************************************************************************************
 * Private interface
 ***********************************************************************************************************/
// Fills up an existing transaction with the serialized data in bytes
// Return non 0 on success
size_t transaction_deserialize_trits(iota_transaction_t transaction,
                                     const flex_trit_t *trits) {
  flex_trit_t partial[FLEX_TRIT_SIZE_81];
  trit_t buffer[81];
  size_t offset = 0;
  memset(transaction, 0, sizeof(struct _iota_transaction));
  flex_trit_array_slice(transaction->signature_or_message, trits,
                        offset, 6561);
  offset += 6561;
  flex_trit_array_slice(transaction->address, trits,
                        offset, 243);
  offset += 243;
  flex_trit_array_slice(partial, trits, offset, 81);
  flex_trit_array_to_int8(buffer, partial, 81);
  transaction_set_value(transaction, trits_to_long(buffer, 81));
  offset += 81;
  flex_trit_array_slice(transaction->obsolete_tag, trits,
                        offset, 81);
  offset += 81;
  flex_trit_array_slice(partial, trits, offset, 27);
  flex_trit_array_to_int8(buffer, partial, 27);
  transaction_set_timestamp(transaction, trits_to_long(buffer, 27));
  offset += 27;
  flex_trit_array_slice(partial, trits, offset, 27);
  flex_trit_array_to_int8(buffer, partial, 27);
  transaction_set_current_index(transaction, trits_to_long(buffer, 27));
  offset += 27;
  flex_trit_array_slice(partial, trits, offset, 27);
  flex_trit_array_to_int8(buffer, partial, 27);
  transaction_set_last_index(transaction, trits_to_long(buffer, 27));
  offset += 27;
  flex_trit_array_slice(transaction->bundle, trits,
                        offset, 243);
  offset += 243;
  flex_trit_array_slice(transaction->trunk, trits,
                        offset, 243);
  offset += 243;
  flex_trit_array_slice(transaction->branch, trits,
                        offset, 243);
  offset += 243;
  flex_trit_array_slice(transaction->tag, trits,
                        offset, 81);
  offset += 81;
  flex_trit_array_slice(partial, trits, offset, 27);
  flex_trit_array_to_int8(buffer, partial, 27);
  transaction_set_attachment_timestamp(transaction,
                                       trits_to_long(buffer, 27));
  offset += 27;
  flex_trit_array_slice(partial, trits, offset, 27);
  flex_trit_array_to_int8(buffer, partial, 27);
  transaction_set_attachment_timestamp_lower(transaction,
                                             trits_to_long(buffer, 27));
  offset += 27;
  flex_trit_array_slice(partial, trits, offset, 27);
  flex_trit_array_to_int8(buffer, partial, 27);
  transaction_set_attachment_timestamp_upper(transaction,
                                             trits_to_long(buffer, 27));
  offset += 27;
  flex_trit_array_slice(transaction->nonce, trits,
                        offset, 81);
  offset += 81;
  return offset;
}

size_t _long_to_flex_trit(int64_t value, flex_trit_t *trits) {
  trit_t buffer[81];
  memset(buffer, 0, 81);
  size_t long_size = long_to_trits(value, buffer);
  int8_to_flex_trit_array(trits, buffer, long_size);
  return long_size;
}
// Serialize an existing transaction
// Return non 0 on success
size_t transaction_serialize_to_trits(const iota_transaction_t transaction,
                                      flex_trit_t *trits) {
  flex_trit_t partial[FLEX_TRIT_SIZE_81];
  size_t offset = 0, long_size;
  size_t num_bytes = flex_trits_num_for_trits(8019);
  memset(trits, 0, num_bytes);

  flex_trit_array_insert(trits, transaction->signature_or_message, offset, 6561);
  offset += 6561;
  flex_trit_array_insert(trits, transaction->address, offset, 243);
  offset += 243;
  long_size = _long_to_flex_trit(transaction->value, partial);
  flex_trit_array_insert(trits, partial, offset, long_size);
  offset += 81;
  flex_trit_array_insert(trits, transaction->obsolete_tag, offset, 81);
  offset += 81;
  long_size = _long_to_flex_trit(transaction->timestamp, partial);
  flex_trit_array_insert(trits, partial, offset, long_size);
  offset += 27;
  long_size = _long_to_flex_trit(transaction->current_index, partial);
  flex_trit_array_insert(trits, partial, offset, long_size);
  offset += 27;
  long_size = _long_to_flex_trit(transaction->last_index, partial);
  flex_trit_array_insert(trits, partial, offset, long_size);
  offset += 27;
  flex_trit_array_insert(trits, transaction->bundle, offset, 243);
  offset += 243;
  flex_trit_array_insert(trits, transaction->trunk, offset, 243);
  offset += 243;
  flex_trit_array_insert(trits, transaction->branch, offset, 243);
  offset += 243;
  flex_trit_array_insert(trits, transaction->tag, offset, 81);
  offset += 81;
  long_size = _long_to_flex_trit(transaction->attachment_timestamp, partial);
  flex_trit_array_insert(trits, partial, offset, long_size);
  offset += 27;
  long_size = _long_to_flex_trit(transaction->attachment_timestamp_lower, partial);
  flex_trit_array_insert(trits, partial, offset, long_size);
  offset += 27;
  long_size = _long_to_flex_trit(transaction->attachment_timestamp_upper, partial);
  flex_trit_array_insert(trits, partial, offset, long_size);
  offset += 27;
  flex_trit_array_insert(trits, transaction->nonce, offset, 81);
  offset += 81;
  return offset;
}

/***********************************************************************************************************
 * Public interface
 ***********************************************************************************************************/
/***********************************************************************************************************
 * Accessors
 ***********************************************************************************************************/
// Get the transaction signature
flex_trit_t *transaction_signature(iota_transaction_t transaction) {
  return transaction->signature_or_message;
}

// Set the transaction signature (copy argument)
void transaction_set_signature(iota_transaction_t transaction,
                               const flex_trit_t *signature) {
  memcpy(transaction->signature_or_message, signature,
         sizeof(transaction->signature_or_message));
}

// Get the transaction message
flex_trit_t *transaction_message(iota_transaction_t transaction) {
  return transaction->signature_or_message;
}

// Set the transaction message (copy argument)
void transaction_set_message(iota_transaction_t transaction,
                             const flex_trit_t *message) {
  memcpy(transaction->signature_or_message, message,
         sizeof(transaction->signature_or_message));
}

// Get the transaction address
flex_trit_t *transaction_address(iota_transaction_t transaction) {
  return transaction->address;
}

// Set the transaction address (copy argument)
void transaction_set_address(iota_transaction_t transaction,
                             const flex_trit_t *address) {
  memcpy(transaction->address, address, sizeof(transaction->address));
}

// Get the transaction value
int64_t transaction_value(iota_transaction_t transaction) {
  return transaction->value;
}

// Set the transaction value
void transaction_set_value(iota_transaction_t transaction, int64_t value) {
  transaction->value = value;
}

// Get the transaction obsolete tag
flex_trit_t *transaction_obsolete_tag(iota_transaction_t transaction) {
  return transaction->obsolete_tag;
}

// Set the transaction obsolete tag
void transaction_set_obsolete_tag(iota_transaction_t transaction,
                                  const flex_trit_t *obsolete_tag) {
  memcpy(transaction->obsolete_tag, obsolete_tag,
         sizeof(transaction->obsolete_tag));
}

// Get the transaction timestamp
uint64_t transaction_timestamp(iota_transaction_t transaction) {
  return transaction->timestamp;
}

// Set the transaction timestamp
void transaction_set_timestamp(iota_transaction_t transaction,
                               uint64_t timestamp) {
  transaction->timestamp = timestamp;
}

// Get the transaction current index
int64_t transaction_current_index(iota_transaction_t transaction) {
  return transaction->current_index;
}

// Set the transaction current index
void transaction_set_current_index(iota_transaction_t transaction,
                                   int64_t index) {
  transaction->current_index = index;
}

// Get the transaction last index
int64_t transaction_last_index(iota_transaction_t transaction) {
  return transaction->last_index;
}

// Set the transaction last index
void transaction_set_last_index(iota_transaction_t transaction, int64_t index) {
  transaction->last_index = index;
}

// Get the transaction bundle
flex_trit_t *transaction_bundle(iota_transaction_t transaction) {
  return transaction->bundle;
}

// Set the transaction bundle (copy argument)
void transaction_set_bundle(iota_transaction_t transaction,
                            const flex_trit_t *bundle) {
  memcpy(transaction->bundle, bundle, sizeof(transaction->bundle));
}

// Get the transaction trunk
flex_trit_t *transaction_trunk(iota_transaction_t transaction) {
  return transaction->trunk;
}

// Set the transaction trunk (copy argument)
void transaction_set_trunk(iota_transaction_t transaction,
                           const flex_trit_t *trunk) {
  memcpy(transaction->trunk, trunk, sizeof(transaction->trunk));
}

// Get the transaction branch
flex_trit_t *transaction_branch(iota_transaction_t transaction) {
  return transaction->branch;
}

// Set the transaction branch (copy argument)
void transaction_set_branch(iota_transaction_t transaction,
                            const flex_trit_t *branch) {
  memcpy(transaction->branch, branch, sizeof(transaction->branch));
}

// Get the transaction tag
flex_trit_t *transaction_tag(iota_transaction_t transaction) {
  return transaction->tag;
}

// Set the transaction tag (copy argument)
void transaction_set_tag(iota_transaction_t transaction, const flex_trit_t *tag) {
  memcpy(transaction->tag, tag, sizeof(transaction->tag));
}

// Get the transaction attachement timestamp
int64_t transaction_attachment_timestamp(iota_transaction_t transaction) {
  return transaction->attachment_timestamp;
}

// Set the transaction attachement timestamp
void transaction_set_attachment_timestamp(iota_transaction_t transaction,
                                          int64_t timestamp) {
  transaction->attachment_timestamp = timestamp;
}

// Get the transaction attachement timestamp lower
int64_t transaction_attachment_timestamp_lower(iota_transaction_t transaction) {
  return transaction->attachment_timestamp_lower;
}

// Set the transaction attachement timestamp lower
void transaction_set_attachment_timestamp_lower(iota_transaction_t transaction,
                                                int64_t timestamp) {
  transaction->attachment_timestamp_lower = timestamp;
}

// Get the transaction attachement timestamp upper
int64_t transaction_attachment_timestamp_upper(iota_transaction_t transaction) {
  return transaction->attachment_timestamp_upper;
}

// Set the transaction attachement timestamp upper
void transaction_set_attachment_timestamp_upper(iota_transaction_t transaction,
                                                int64_t timestamp) {
  transaction->attachment_timestamp_upper = timestamp;
}

// Get the transaction nonce
flex_trit_t *transaction_nonce(iota_transaction_t transaction) {
  return transaction->nonce;
}

// Set the transaction nonce (copy argument)
void transaction_set_nonce(iota_transaction_t transaction,
                           const flex_trit_t *nonce) {
  memcpy(transaction->nonce, nonce, sizeof(transaction->nonce));
}

/***********************************************************************************************************
 * Constructors
 ***********************************************************************************************************/
// Creates and returns a new transaction without data
iota_transaction_t transaction_new(void) {
  iota_transaction_t transaction;
  transaction = (iota_transaction_t)malloc(sizeof(struct _iota_transaction));
  if (!transaction) {
    // errno = IOTA_OUT_OF_MEMORY
  }
  memset(transaction, 0, sizeof(struct _iota_transaction));
  return transaction;
}

// Creates and returns a new transaction from serialized data
// Returns NULL if failed
iota_transaction_t transaction_deserialize(const flex_trit_t *trits) {
  iota_transaction_t transaction;
  transaction = transaction_new();
  if (!transaction) {
    // errno = IOTA_OUT_OF_MEMORY
    return NULL;
  }
  if (!transaction_deserialize_from_trits(transaction, trits)) {
    // errno = IOTA_SOME_ERROR
    transaction_free(transaction);
    return NULL;
  }
  return transaction;
}

/***********************************************************************************************************
 * Serialization
 ***********************************************************************************************************/
// Returns the serialized data from an existing transaction
// Returns NULL if failed
flex_trit_t *transaction_serialize(const iota_transaction_t transaction) {
  size_t num_bytes = flex_trits_num_for_trits(8019);
  flex_trit_t *serialized_value = (flex_trit_t *)malloc(sizeof(flex_trit_t) * num_bytes);
  if (!serialized_value) {
    // errno = IOTA_OUT_OF_MEMORY
    return NULL;
  }
  if (!transaction_serialize_on_trits(transaction, serialized_value)) {
    free(serialized_value);
    serialized_value = NULL;
  }
  return serialized_value;
}

// Places the serialized data from an existing transaction in pre-allocated
// bytes Returns non 0 on success
size_t transaction_serialize_on_trits(const iota_transaction_t transaction,
                                      flex_trit_t *trits) {
  return transaction_serialize_to_trits(transaction, trits);
}

/***********************************************************************************************************
 * Deserialization
 ***********************************************************************************************************/
// Fills up an existing transaction with the serialized data in trits - returns
// non 0 on success
size_t transaction_deserialize_from_trits(iota_transaction_t transaction,
                                          const flex_trit_t *trits) {
  return transaction_deserialize_trits(transaction, trits);
}

/***********************************************************************************************************
 * Destructor
 ***********************************************************************************************************/
// Free an existing transaction - compatible with free()
void transaction_free(void *t) { free(t); }
