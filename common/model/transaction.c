/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#include <stdlib.h>
#include <string.h>

#include "common/curl-p/digest.h"
#include "common/model/transaction.h"
#include "common/trinary/trit_long.h"

/***********************************************************************************************************
 * Transaction
 ***********************************************************************************************************/
// To obfuscate the model, the structure definition comes here
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
  flex_trits_slice(transaction->data.signature_or_message, NUM_TRITS_SIGNATURE,
                   trits, NUM_TRITS_SERIALIZED_TRANSACTION, offset,
                   NUM_TRITS_SIGNATURE);
  offset += NUM_TRITS_SIGNATURE;
  flex_trits_slice(transaction->essence.address, NUM_TRITS_ADDRESS, trits,
                   NUM_TRITS_SERIALIZED_TRANSACTION, offset, NUM_TRITS_ADDRESS);
  offset += NUM_TRITS_ADDRESS;
  flex_trits_slice(partial, NUM_TRITS_VALUE, trits,
                   NUM_TRITS_SERIALIZED_TRANSACTION, offset, NUM_TRITS_VALUE);
  flex_trits_to_trits(buffer, NUM_TRITS_VALUE, partial, NUM_TRITS_VALUE,
                      NUM_TRITS_VALUE);
  transaction_set_value(transaction, trits_to_long(buffer, NUM_TRITS_VALUE));
  offset += NUM_TRITS_VALUE;
  flex_trits_slice(transaction->essence.obsolete_tag, NUM_TRITS_OBSOLETE_TAG,
                   trits, NUM_TRITS_SERIALIZED_TRANSACTION, offset,
                   NUM_TRITS_OBSOLETE_TAG);
  offset += NUM_TRITS_OBSOLETE_TAG;
  flex_trits_slice(partial, NUM_TRITS_TIMESTAMP, trits,
                   NUM_TRITS_SERIALIZED_TRANSACTION, offset,
                   NUM_TRITS_TIMESTAMP);
  flex_trits_to_trits(buffer, NUM_TRITS_TIMESTAMP, partial, NUM_TRITS_TIMESTAMP,
                      NUM_TRITS_TIMESTAMP);
  transaction_set_timestamp(transaction,
                            trits_to_long(buffer, NUM_TRITS_TIMESTAMP));
  offset += NUM_TRITS_TIMESTAMP;
  flex_trits_slice(partial, NUM_TRITS_CURRENT_INDEX, trits,
                   NUM_TRITS_SERIALIZED_TRANSACTION, offset,
                   NUM_TRITS_CURRENT_INDEX);
  flex_trits_to_trits(buffer, NUM_TRITS_CURRENT_INDEX, partial,
                      NUM_TRITS_CURRENT_INDEX, NUM_TRITS_CURRENT_INDEX);
  transaction_set_current_index(transaction, trits_to_long(buffer, 27));
  offset += NUM_TRITS_CURRENT_INDEX;
  flex_trits_slice(partial, NUM_TRITS_LAST_INDEX, trits,
                   NUM_TRITS_SERIALIZED_TRANSACTION, offset,
                   NUM_TRITS_LAST_INDEX);
  flex_trits_to_trits(buffer, NUM_TRITS_LAST_INDEX, partial,
                      NUM_TRITS_LAST_INDEX, NUM_TRITS_LAST_INDEX);
  transaction_set_last_index(transaction,
                             trits_to_long(buffer, NUM_TRITS_LAST_INDEX));
  offset += NUM_TRITS_LAST_INDEX;
  flex_trits_slice(transaction->consensus.bundle, NUM_TRITS_BUNDLE, trits,
                   NUM_TRITS_SERIALIZED_TRANSACTION, offset, NUM_TRITS_BUNDLE);
  offset += NUM_TRITS_BUNDLE;
  flex_trits_slice(transaction->attachement.trunk, NUM_TRITS_TRUNK, trits,
                   NUM_TRITS_SERIALIZED_TRANSACTION, offset, NUM_TRITS_TRUNK);
  offset += NUM_TRITS_TRUNK;
  flex_trits_slice(transaction->attachement.branch, NUM_TRITS_BRANCH, trits,
                   NUM_TRITS_SERIALIZED_TRANSACTION, offset, NUM_TRITS_BRANCH);
  offset += NUM_TRITS_BRANCH;
  flex_trits_slice(transaction->attachement.tag, NUM_TRITS_TAG, trits,
                   NUM_TRITS_SERIALIZED_TRANSACTION, offset, NUM_TRITS_TAG);
  offset += NUM_TRITS_TAG;
  flex_trits_slice(partial, NUM_TRITS_ATTACHMENT_TIMESTAMP, trits,
                   NUM_TRITS_SERIALIZED_TRANSACTION, offset,
                   NUM_TRITS_ATTACHMENT_TIMESTAMP);
  flex_trits_to_trits(buffer, NUM_TRITS_ATTACHMENT_TIMESTAMP, partial,
                      NUM_TRITS_ATTACHMENT_TIMESTAMP,
                      NUM_TRITS_ATTACHMENT_TIMESTAMP);
  transaction_set_attachment_timestamp(
      transaction, trits_to_long(buffer, NUM_TRITS_ATTACHMENT_TIMESTAMP));
  offset += NUM_TRITS_ATTACHMENT_TIMESTAMP;
  flex_trits_slice(partial, NUM_TRITS_ATTACHMENT_TIMESTAMP_LOWER, trits,
                   NUM_TRITS_SERIALIZED_TRANSACTION, offset,
                   NUM_TRITS_ATTACHMENT_TIMESTAMP_LOWER);
  flex_trits_to_trits(buffer, NUM_TRITS_ATTACHMENT_TIMESTAMP_LOWER, partial,
                      NUM_TRITS_ATTACHMENT_TIMESTAMP_LOWER,
                      NUM_TRITS_ATTACHMENT_TIMESTAMP_LOWER);
  transaction_set_attachment_timestamp_lower(
      transaction, trits_to_long(buffer, NUM_TRITS_ATTACHMENT_TIMESTAMP_LOWER));
  offset += NUM_TRITS_ATTACHMENT_TIMESTAMP_LOWER;
  flex_trits_slice(partial, NUM_TRITS_ATTACHMENT_TIMESTAMP_UPPER, trits,
                   NUM_TRITS_SERIALIZED_TRANSACTION, offset,
                   NUM_TRITS_ATTACHMENT_TIMESTAMP_UPPER);
  flex_trits_to_trits(buffer, NUM_TRITS_ATTACHMENT_TIMESTAMP_UPPER, partial,
                      NUM_TRITS_ATTACHMENT_TIMESTAMP_UPPER,
                      NUM_TRITS_ATTACHMENT_TIMESTAMP_UPPER);
  transaction_set_attachment_timestamp_upper(
      transaction, trits_to_long(buffer, NUM_TRITS_ATTACHMENT_TIMESTAMP_UPPER));
  offset += NUM_TRITS_ATTACHMENT_TIMESTAMP_UPPER;
  flex_trits_slice(transaction->attachement.nonce, NUM_TRITS_NONCE, trits,
                   NUM_TRITS_SERIALIZED_TRANSACTION, offset, NUM_TRITS_NONCE);
  offset += NUM_TRITS_NONCE;

  // Compute the transaction hash
  // FIXME(thibault) Waiting for cryptographic functions to handle flex_trits
  Curl curl;
  init_curl(&curl);
  curl.type = CURL_P_81;
  trit_t tx_trits[NUM_TRITS_SERIALIZED_TRANSACTION];
  trit_t hash[NUM_TRITS_HASH];
  flex_trits_to_trits(tx_trits, NUM_TRITS_SERIALIZED_TRANSACTION, trits, offset,
                      offset);
  curl_digest(tx_trits, NUM_TRITS_SERIALIZED_TRANSACTION, hash, &curl);
  flex_trits_from_trits(transaction->consensus.hash, NUM_TRITS_HASH, hash,
                        NUM_TRITS_HASH, NUM_TRITS_HASH);
  return offset;
}

size_t _long_to_flex_trit(int64_t value, flex_trit_t *trits) {
  trit_t buffer[NUM_TRITS_VALUE];
  memset(buffer, 0, NUM_TRITS_VALUE);
  size_t long_size = long_to_trits(value, buffer);
  flex_trits_from_trits(trits, NUM_TRITS_VALUE, buffer, NUM_TRITS_VALUE,
                        NUM_TRITS_VALUE);
  return long_size;
}
// Serialize an existing transaction
// Return non 0 on success
size_t transaction_serialize_to_flex_trits(const iota_transaction_t transaction,
                                           flex_trit_t *trits) {
  flex_trit_t partial[FLEX_TRIT_SIZE_81];
  size_t offset = 0, long_size;
  memset(trits, FLEX_TRIT_NULL_VALUE, FLEX_TRIT_SIZE_8019);

  flex_trits_insert(trits, NUM_TRITS_SERIALIZED_TRANSACTION,
                    transaction->data.signature_or_message, NUM_TRITS_SIGNATURE,
                    offset, NUM_TRITS_SIGNATURE);
  offset += NUM_TRITS_SIGNATURE;
  flex_trits_insert(trits, NUM_TRITS_SERIALIZED_TRANSACTION,
                    transaction->essence.address, NUM_TRITS_ADDRESS, offset,
                    NUM_TRITS_ADDRESS);
  offset += NUM_TRITS_ADDRESS;
  long_size = _long_to_flex_trit(transaction->essence.value, partial);
  flex_trits_insert(trits, NUM_TRITS_SERIALIZED_TRANSACTION, partial,
                    NUM_TRITS_VALUE, offset, NUM_TRITS_VALUE);
  offset += NUM_TRITS_VALUE;
  flex_trits_insert(trits, NUM_TRITS_SERIALIZED_TRANSACTION,
                    transaction->essence.obsolete_tag, NUM_TRITS_OBSOLETE_TAG,
                    offset, NUM_TRITS_OBSOLETE_TAG);
  offset += NUM_TRITS_OBSOLETE_TAG;
  long_size = _long_to_flex_trit(transaction->essence.timestamp, partial);
  flex_trits_insert(trits, NUM_TRITS_SERIALIZED_TRANSACTION, partial,
                    NUM_TRITS_TIMESTAMP, offset, NUM_TRITS_TIMESTAMP);
  offset += NUM_TRITS_TIMESTAMP;
  long_size = _long_to_flex_trit(transaction->essence.current_index, partial);
  flex_trits_insert(trits, NUM_TRITS_SERIALIZED_TRANSACTION, partial,
                    NUM_TRITS_CURRENT_INDEX, offset, NUM_TRITS_CURRENT_INDEX);
  offset += NUM_TRITS_CURRENT_INDEX;
  long_size = _long_to_flex_trit(transaction->essence.last_index, partial);
  flex_trits_insert(trits, NUM_TRITS_SERIALIZED_TRANSACTION, partial,
                    NUM_TRITS_LAST_INDEX, offset, NUM_TRITS_LAST_INDEX);
  offset += NUM_TRITS_LAST_INDEX;
  flex_trits_insert(trits, NUM_TRITS_SERIALIZED_TRANSACTION,
                    transaction->consensus.bundle, NUM_TRITS_BUNDLE, offset,
                    NUM_TRITS_BUNDLE);
  offset += NUM_TRITS_BUNDLE;
  flex_trits_insert(trits, NUM_TRITS_SERIALIZED_TRANSACTION,
                    transaction->attachement.trunk, NUM_TRITS_TRUNK, offset,
                    NUM_TRITS_TRUNK);
  offset += NUM_TRITS_TRUNK;
  flex_trits_insert(trits, NUM_TRITS_SERIALIZED_TRANSACTION,
                    transaction->attachement.branch, NUM_TRITS_BRANCH, offset,
                    NUM_TRITS_BRANCH);
  offset += NUM_TRITS_BRANCH;
  flex_trits_insert(trits, NUM_TRITS_SERIALIZED_TRANSACTION,
                    transaction->attachement.tag, NUM_TRITS_TAG, offset,
                    NUM_TRITS_TAG);
  offset += NUM_TRITS_TAG;
  long_size = _long_to_flex_trit(transaction->attachement.attachment_timestamp,
                                 partial);
  flex_trits_insert(trits, NUM_TRITS_SERIALIZED_TRANSACTION, partial,
                    NUM_TRITS_ATTACHMENT_TIMESTAMP, offset,
                    NUM_TRITS_ATTACHMENT_TIMESTAMP);
  offset += NUM_TRITS_ATTACHMENT_TIMESTAMP;
  long_size = _long_to_flex_trit(
      transaction->attachement.attachment_timestamp_lower, partial);
  flex_trits_insert(trits, NUM_TRITS_SERIALIZED_TRANSACTION, partial,
                    NUM_TRITS_ATTACHMENT_TIMESTAMP_LOWER, offset,
                    NUM_TRITS_ATTACHMENT_TIMESTAMP_LOWER);
  offset += NUM_TRITS_ATTACHMENT_TIMESTAMP_LOWER;
  long_size = _long_to_flex_trit(
      transaction->attachement.attachment_timestamp_upper, partial);
  flex_trits_insert(trits, NUM_TRITS_SERIALIZED_TRANSACTION, partial,
                    NUM_TRITS_ATTACHMENT_TIMESTAMP_UPPER, offset,
                    NUM_TRITS_ATTACHMENT_TIMESTAMP_UPPER);
  offset += NUM_TRITS_ATTACHMENT_TIMESTAMP_UPPER;
  flex_trits_insert(trits, NUM_TRITS_SERIALIZED_TRANSACTION,
                    transaction->attachement.nonce, NUM_TRITS_NONCE, offset,
                    NUM_TRITS_NONCE);
  offset += NUM_TRITS_NONCE;
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
  return transaction->data.signature_or_message;
}

// Set the transaction signature (copy argument)
void transaction_set_signature(iota_transaction_t transaction,
                               const flex_trit_t *signature) {
  memcpy(transaction->data.signature_or_message, signature,
         sizeof(transaction->data.signature_or_message));
}

// Get the transaction message
flex_trit_t *transaction_message(iota_transaction_t transaction) {
  return transaction->data.signature_or_message;
}

// Set the transaction message (copy argument)
void transaction_set_message(iota_transaction_t transaction,
                             const flex_trit_t *message) {
  memcpy(transaction->data.signature_or_message, message,
         sizeof(transaction->data.signature_or_message));
}

// Get the transaction address
flex_trit_t *transaction_address(iota_transaction_t transaction) {
  return transaction->essence.address;
}

// Set the transaction address (copy argument)
void transaction_set_address(iota_transaction_t transaction,
                             const flex_trit_t *address) {
  memcpy(transaction->essence.address, address,
         sizeof(transaction->essence.address));
}

// Get the transaction value
int64_t transaction_value(iota_transaction_t transaction) {
  return transaction->essence.value;
}

// Set the transaction value
void transaction_set_value(iota_transaction_t transaction, int64_t value) {
  transaction->essence.value = value;
}

// Get the transaction obsolete tag
flex_trit_t *transaction_obsolete_tag(iota_transaction_t transaction) {
  return transaction->essence.obsolete_tag;
}

// Set the transaction obsolete tag
void transaction_set_obsolete_tag(iota_transaction_t transaction,
                                  const flex_trit_t *obsolete_tag) {
  memcpy(transaction->essence.obsolete_tag, obsolete_tag,
         sizeof(transaction->essence.obsolete_tag));
}

// Get the transaction timestamp
uint64_t transaction_timestamp(iota_transaction_t transaction) {
  return transaction->essence.timestamp;
}

// Set the transaction timestamp
void transaction_set_timestamp(iota_transaction_t transaction,
                               uint64_t timestamp) {
  transaction->essence.timestamp = timestamp;
}

// Get the transaction current index
int64_t transaction_current_index(iota_transaction_t transaction) {
  return transaction->essence.current_index;
}

// Set the transaction current index
void transaction_set_current_index(iota_transaction_t transaction,
                                   int64_t index) {
  transaction->essence.current_index = index;
}

// Get the transaction last index
int64_t transaction_last_index(iota_transaction_t transaction) {
  return transaction->essence.last_index;
}

// Set the transaction last index
void transaction_set_last_index(iota_transaction_t transaction, int64_t index) {
  transaction->essence.last_index = index;
}

// Get the transaction bundle
flex_trit_t *transaction_bundle(iota_transaction_t transaction) {
  return transaction->consensus.bundle;
}

// Set the transaction bundle (copy argument)
void transaction_set_bundle(iota_transaction_t transaction,
                            const flex_trit_t *bundle) {
  memcpy(transaction->consensus.bundle, bundle,
         sizeof(transaction->consensus.bundle));
}

// Get the transaction trunk
flex_trit_t *transaction_trunk(iota_transaction_t transaction) {
  return transaction->attachement.trunk;
}

// Set the transaction trunk (copy argument)
void transaction_set_trunk(iota_transaction_t transaction,
                           const flex_trit_t *trunk) {
  memcpy(transaction->attachement.trunk, trunk,
         sizeof(transaction->attachement.trunk));
}

// Get the transaction branch
flex_trit_t *transaction_branch(iota_transaction_t transaction) {
  return transaction->attachement.branch;
}

// Set the transaction branch (copy argument)
void transaction_set_branch(iota_transaction_t transaction,
                            const flex_trit_t *branch) {
  memcpy(transaction->attachement.branch, branch,
         sizeof(transaction->attachement.branch));
}

// Get the transaction tag
flex_trit_t *transaction_tag(iota_transaction_t transaction) {
  return transaction->attachement.tag;
}

// Set the transaction tag (copy argument)
void transaction_set_tag(iota_transaction_t transaction,
                         const flex_trit_t *tag) {
  memcpy(transaction->attachement.tag, tag,
         sizeof(transaction->attachement.tag));
}

// Get the transaction attachment timestamp
int64_t transaction_attachment_timestamp(iota_transaction_t transaction) {
  return transaction->attachement.attachment_timestamp;
}

// Set the transaction attachment timestamp
void transaction_set_attachment_timestamp(iota_transaction_t transaction,
                                          int64_t timestamp) {
  transaction->attachement.attachment_timestamp = timestamp;
}

// Get the transaction attachment timestamp lower
int64_t transaction_attachment_timestamp_lower(iota_transaction_t transaction) {
  return transaction->attachement.attachment_timestamp_lower;
}

// Set the transaction attachment timestamp lower
void transaction_set_attachment_timestamp_lower(iota_transaction_t transaction,
                                                int64_t timestamp) {
  transaction->attachement.attachment_timestamp_lower = timestamp;
}

// Get the transaction attachment timestamp upper
int64_t transaction_attachment_timestamp_upper(iota_transaction_t transaction) {
  return transaction->attachement.attachment_timestamp_upper;
}

// Set the transaction attachment timestamp upper
void transaction_set_attachment_timestamp_upper(iota_transaction_t transaction,
                                                int64_t timestamp) {
  transaction->attachement.attachment_timestamp_upper = timestamp;
}

// Get the transaction nonce
flex_trit_t *transaction_nonce(iota_transaction_t transaction) {
  return transaction->attachement.nonce;
}

// Set the transaction nonce (copy argument)
void transaction_set_nonce(iota_transaction_t transaction,
                           const flex_trit_t *nonce) {
  memcpy(transaction->attachement.nonce, nonce,
         sizeof(transaction->attachement.nonce));
}

// Get the transaction hash
flex_trit_t *transaction_hash(iota_transaction_t transaction) {
  return transaction->consensus.hash;
}

// Set the transaction hash (copy argument)
void transaction_set_hash(iota_transaction_t transaction,
                          const flex_trit_t *hash) {
  memcpy(transaction->consensus.hash, hash,
         sizeof(transaction->consensus.hash));
}

uint64_t transaction_snapshot_index(iota_transaction_t transaction) {
  return transaction->metadata.snapshot_index;
}
// Set the transaction snapshot index
void transaction_set_snapshot_index(iota_transaction_t transaction,
                                    uint64_t snapshot_index) {
  transaction->metadata.snapshot_index = snapshot_index;
}

bool transaction_solid(iota_transaction_t transaction) {
  return transaction->metadata.solid;
}
// Set the transaction solid state
void transaction_set_solid(iota_transaction_t transaction, bool state) {
  transaction->metadata.solid = state;
}

// Reset all transaction fields
void transaction_reset(iota_transaction_t transaction) {
  memset(transaction, 0, sizeof(struct _iota_transaction));
  memset(transaction->data.signature_or_message, FLEX_TRIT_NULL_VALUE,
         sizeof(transaction->data.signature_or_message));
  memset(transaction->essence.address, FLEX_TRIT_NULL_VALUE,
         sizeof(transaction->essence.address));
  memset(transaction->essence.obsolete_tag, FLEX_TRIT_NULL_VALUE,
         sizeof(transaction->essence.obsolete_tag));
  memset(transaction->consensus.bundle, FLEX_TRIT_NULL_VALUE,
         sizeof(transaction->consensus.bundle));
  memset(transaction->attachement.trunk, FLEX_TRIT_NULL_VALUE,
         sizeof(transaction->attachement.trunk));
  memset(transaction->attachement.branch, FLEX_TRIT_NULL_VALUE,
         sizeof(transaction->attachement.branch));
  memset(transaction->attachement.tag, FLEX_TRIT_NULL_VALUE,
         sizeof(transaction->attachement.tag));
  memset(transaction->attachement.nonce, FLEX_TRIT_NULL_VALUE,
         sizeof(transaction->attachement.nonce));
  memset(transaction->consensus.hash, FLEX_TRIT_NULL_VALUE,
         sizeof(transaction->consensus.hash));
}

uint8_t transaction_weight_magnitude(const iota_transaction_t transaction) {
  uint8_t num_trailing_null_values = 0;
  uint8_t pos = FLEX_TRIT_SIZE_243;

  while (pos-- > 0 &&
         transaction->consensus.hash[pos] == FLEX_TRIT_NULL_VALUE) {
    num_trailing_null_values += NUM_TRITS_PER_FLEX_TRIT;
  }

  if (pos > 0) {
    trit_t one_trit_buffer[NUM_TRITS_PER_FLEX_TRIT];
    flex_trits_to_trits(one_trit_buffer, NUM_TRITS_PER_FLEX_TRIT,
                        &transaction->consensus.hash[pos],
                        NUM_TRITS_PER_FLEX_TRIT, NUM_TRITS_PER_FLEX_TRIT);

    pos = NUM_TRITS_PER_FLEX_TRIT;
    while (pos-- > 0 && one_trit_buffer[pos] == 0) {
      ++num_trailing_null_values;
    }
  }

  return num_trailing_null_values;
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
  transaction_reset(transaction);
  transaction->metadata.snapshot_index = 0;
  transaction->metadata.solid = 0;
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
  size_t num_bytes = FLEX_TRIT_SIZE_8019;
  flex_trit_t *serialized_value =
      (flex_trit_t *)malloc(sizeof(flex_trit_t) * num_bytes);
  if (!serialized_value) {
    // errno = IOTA_OUT_OF_MEMORY
    return NULL;
  }
  if (!transaction_serialize_on_flex_trits(transaction, serialized_value)) {
    free(serialized_value);
    serialized_value = NULL;
  }
  return serialized_value;
}

// Places the serialized data from an existing transaction in pre-allocated
// bytes Returns non 0 on success
size_t transaction_serialize_on_flex_trits(const iota_transaction_t transaction,
                                           flex_trit_t *trits) {
  return transaction_serialize_to_flex_trits(transaction, trits);
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
void transaction_free(iota_transaction_t t) { free(t); }
