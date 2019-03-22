/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#include <stdlib.h>
#include <string.h>
#ifdef DEBUG
#include <inttypes.h>
#include <stdio.h>
#endif

#include "common/crypto/curl-p/digest.h"
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
size_t transaction_deserialize_trits(iota_transaction_t *const transaction, flex_trit_t const *const trits,
                                     bool const compute_hash) {
  flex_trit_t partial[FLEX_TRIT_SIZE_81];
  trit_t buffer[81];
  size_t offset = 0;
  flex_trits_slice(transaction->data.signature_or_message, NUM_TRITS_SIGNATURE, trits, NUM_TRITS_SERIALIZED_TRANSACTION,
                   offset, NUM_TRITS_SIGNATURE);
  transaction->loaded_columns_mask.data |= MASK_DATA_SIG_OR_MSG;
  offset += NUM_TRITS_SIGNATURE;
  flex_trits_slice(transaction->essence.address, NUM_TRITS_ADDRESS, trits, NUM_TRITS_SERIALIZED_TRANSACTION, offset,
                   NUM_TRITS_ADDRESS);
  transaction->loaded_columns_mask.essence |= MASK_ESSENCE_ADDRESS;
  offset += NUM_TRITS_ADDRESS;
  flex_trits_slice(partial, NUM_TRITS_VALUE, trits, NUM_TRITS_SERIALIZED_TRANSACTION, offset, NUM_TRITS_VALUE);
  flex_trits_to_trits(buffer, NUM_TRITS_VALUE, partial, NUM_TRITS_VALUE, NUM_TRITS_VALUE);
  transaction_set_value(transaction, trits_to_long(buffer, NUM_TRITS_VALUE));
  offset += NUM_TRITS_VALUE;
  flex_trits_slice(transaction->essence.obsolete_tag, NUM_TRITS_OBSOLETE_TAG, trits, NUM_TRITS_SERIALIZED_TRANSACTION,
                   offset, NUM_TRITS_OBSOLETE_TAG);
  transaction->loaded_columns_mask.essence |= MASK_ESSENCE_OBSOLETE_TAG;
  offset += NUM_TRITS_OBSOLETE_TAG;
  flex_trits_slice(partial, NUM_TRITS_TIMESTAMP, trits, NUM_TRITS_SERIALIZED_TRANSACTION, offset, NUM_TRITS_TIMESTAMP);
  flex_trits_to_trits(buffer, NUM_TRITS_TIMESTAMP, partial, NUM_TRITS_TIMESTAMP, NUM_TRITS_TIMESTAMP);
  transaction_set_timestamp(transaction, trits_to_long(buffer, NUM_TRITS_TIMESTAMP));
  offset += NUM_TRITS_TIMESTAMP;
  flex_trits_slice(partial, NUM_TRITS_CURRENT_INDEX, trits, NUM_TRITS_SERIALIZED_TRANSACTION, offset,
                   NUM_TRITS_CURRENT_INDEX);
  flex_trits_to_trits(buffer, NUM_TRITS_CURRENT_INDEX, partial, NUM_TRITS_CURRENT_INDEX, NUM_TRITS_CURRENT_INDEX);
  transaction_set_current_index(transaction, trits_to_long(buffer, NUM_TRITS_CURRENT_INDEX));
  offset += NUM_TRITS_CURRENT_INDEX;
  flex_trits_slice(partial, NUM_TRITS_LAST_INDEX, trits, NUM_TRITS_SERIALIZED_TRANSACTION, offset,
                   NUM_TRITS_LAST_INDEX);
  flex_trits_to_trits(buffer, NUM_TRITS_LAST_INDEX, partial, NUM_TRITS_LAST_INDEX, NUM_TRITS_LAST_INDEX);
  transaction_set_last_index(transaction, trits_to_long(buffer, NUM_TRITS_LAST_INDEX));
  offset += NUM_TRITS_LAST_INDEX;
  flex_trits_slice(transaction->essence.bundle, NUM_TRITS_BUNDLE, trits, NUM_TRITS_SERIALIZED_TRANSACTION, offset,
                   NUM_TRITS_BUNDLE);
  transaction->loaded_columns_mask.essence |= MASK_ESSENCE_BUNDLE;
  offset += NUM_TRITS_BUNDLE;
  flex_trits_slice(transaction->attachment.trunk, NUM_TRITS_TRUNK, trits, NUM_TRITS_SERIALIZED_TRANSACTION, offset,
                   NUM_TRITS_TRUNK);
  transaction->loaded_columns_mask.attachment |= MASK_ATTACHMENT_TRUNK;
  offset += NUM_TRITS_TRUNK;
  flex_trits_slice(transaction->attachment.branch, NUM_TRITS_BRANCH, trits, NUM_TRITS_SERIALIZED_TRANSACTION, offset,
                   NUM_TRITS_BRANCH);
  transaction->loaded_columns_mask.attachment |= MASK_ATTACHMENT_BRANCH;
  offset += NUM_TRITS_BRANCH;
  flex_trits_slice(transaction->attachment.tag, NUM_TRITS_TAG, trits, NUM_TRITS_SERIALIZED_TRANSACTION, offset,
                   NUM_TRITS_TAG);
  transaction->loaded_columns_mask.attachment |= MASK_ATTACHMENT_TAG;
  offset += NUM_TRITS_TAG;
  flex_trits_slice(partial, NUM_TRITS_ATTACHMENT_TIMESTAMP, trits, NUM_TRITS_SERIALIZED_TRANSACTION, offset,
                   NUM_TRITS_ATTACHMENT_TIMESTAMP);
  flex_trits_to_trits(buffer, NUM_TRITS_ATTACHMENT_TIMESTAMP, partial, NUM_TRITS_ATTACHMENT_TIMESTAMP,
                      NUM_TRITS_ATTACHMENT_TIMESTAMP);
  transaction_set_attachment_timestamp(transaction, trits_to_long(buffer, NUM_TRITS_ATTACHMENT_TIMESTAMP));
  offset += NUM_TRITS_ATTACHMENT_TIMESTAMP;
  flex_trits_slice(partial, NUM_TRITS_ATTACHMENT_TIMESTAMP_LOWER, trits, NUM_TRITS_SERIALIZED_TRANSACTION, offset,
                   NUM_TRITS_ATTACHMENT_TIMESTAMP_LOWER);
  flex_trits_to_trits(buffer, NUM_TRITS_ATTACHMENT_TIMESTAMP_LOWER, partial, NUM_TRITS_ATTACHMENT_TIMESTAMP_LOWER,
                      NUM_TRITS_ATTACHMENT_TIMESTAMP_LOWER);
  transaction_set_attachment_timestamp_lower(transaction, trits_to_long(buffer, NUM_TRITS_ATTACHMENT_TIMESTAMP_LOWER));
  offset += NUM_TRITS_ATTACHMENT_TIMESTAMP_LOWER;
  flex_trits_slice(partial, NUM_TRITS_ATTACHMENT_TIMESTAMP_UPPER, trits, NUM_TRITS_SERIALIZED_TRANSACTION, offset,
                   NUM_TRITS_ATTACHMENT_TIMESTAMP_UPPER);
  flex_trits_to_trits(buffer, NUM_TRITS_ATTACHMENT_TIMESTAMP_UPPER, partial, NUM_TRITS_ATTACHMENT_TIMESTAMP_UPPER,
                      NUM_TRITS_ATTACHMENT_TIMESTAMP_UPPER);
  transaction_set_attachment_timestamp_upper(transaction, trits_to_long(buffer, NUM_TRITS_ATTACHMENT_TIMESTAMP_UPPER));
  offset += NUM_TRITS_ATTACHMENT_TIMESTAMP_UPPER;
  flex_trits_slice(transaction->attachment.nonce, NUM_TRITS_NONCE, trits, NUM_TRITS_SERIALIZED_TRANSACTION, offset,
                   NUM_TRITS_NONCE);
  transaction->loaded_columns_mask.attachment |= MASK_ATTACHMENT_NONCE;
  offset += NUM_TRITS_NONCE;

  if (compute_hash) {
    Curl curl;
    curl_init(&curl);
    curl.type = CURL_P_81;
    trit_t tx_trits[NUM_TRITS_SERIALIZED_TRANSACTION];
    trit_t hash[NUM_TRITS_HASH];
    flex_trits_to_trits(tx_trits, NUM_TRITS_SERIALIZED_TRANSACTION, trits, offset, offset);
    curl_digest(tx_trits, NUM_TRITS_SERIALIZED_TRANSACTION, hash, &curl);
    flex_trits_from_trits(transaction->consensus.hash, NUM_TRITS_HASH, hash, NUM_TRITS_HASH, NUM_TRITS_HASH);
    transaction->loaded_columns_mask.consensus |= MASK_CONSENSUS_HASH;
  }

  return offset;
}

static size_t _long_to_flex_trit(int64_t const value, flex_trit_t *const trits) {
  trit_t buffer[NUM_TRITS_VALUE];
  memset(buffer, 0, NUM_TRITS_VALUE);
  size_t long_size = long_to_trits(value, buffer);
  flex_trits_from_trits(trits, NUM_TRITS_VALUE, buffer, NUM_TRITS_VALUE, NUM_TRITS_VALUE);
  return long_size;
}

// Serialize an existing transaction
// Return non 0 on success
size_t transaction_serialize_to_flex_trits(iota_transaction_t const *const transaction, flex_trit_t *const trits) {
  flex_trit_t partial[FLEX_TRIT_SIZE_81];
  size_t offset = 0;

  memset(trits, FLEX_TRIT_NULL_VALUE, FLEX_TRIT_SIZE_8019);

  flex_trits_insert(trits, NUM_TRITS_SERIALIZED_TRANSACTION, transaction->data.signature_or_message,
                    NUM_TRITS_SIGNATURE, offset, NUM_TRITS_SIGNATURE);
  offset += NUM_TRITS_SIGNATURE;
  flex_trits_insert(trits, NUM_TRITS_SERIALIZED_TRANSACTION, transaction->essence.address, NUM_TRITS_ADDRESS, offset,
                    NUM_TRITS_ADDRESS);
  offset += NUM_TRITS_ADDRESS;
  _long_to_flex_trit(transaction->essence.value, partial);
  flex_trits_insert(trits, NUM_TRITS_SERIALIZED_TRANSACTION, partial, NUM_TRITS_VALUE, offset, NUM_TRITS_VALUE);
  offset += NUM_TRITS_VALUE;
  flex_trits_insert(trits, NUM_TRITS_SERIALIZED_TRANSACTION, transaction->essence.obsolete_tag, NUM_TRITS_OBSOLETE_TAG,
                    offset, NUM_TRITS_OBSOLETE_TAG);
  offset += NUM_TRITS_OBSOLETE_TAG;
  _long_to_flex_trit(transaction->essence.timestamp, partial);
  flex_trits_insert(trits, NUM_TRITS_SERIALIZED_TRANSACTION, partial, NUM_TRITS_TIMESTAMP, offset, NUM_TRITS_TIMESTAMP);
  offset += NUM_TRITS_TIMESTAMP;
  _long_to_flex_trit(transaction->essence.current_index, partial);
  flex_trits_insert(trits, NUM_TRITS_SERIALIZED_TRANSACTION, partial, NUM_TRITS_CURRENT_INDEX, offset,
                    NUM_TRITS_CURRENT_INDEX);
  offset += NUM_TRITS_CURRENT_INDEX;
  _long_to_flex_trit(transaction->essence.last_index, partial);
  flex_trits_insert(trits, NUM_TRITS_SERIALIZED_TRANSACTION, partial, NUM_TRITS_LAST_INDEX, offset,
                    NUM_TRITS_LAST_INDEX);
  offset += NUM_TRITS_LAST_INDEX;
  flex_trits_insert(trits, NUM_TRITS_SERIALIZED_TRANSACTION, transaction->essence.bundle, NUM_TRITS_BUNDLE, offset,
                    NUM_TRITS_BUNDLE);
  offset += NUM_TRITS_BUNDLE;
  flex_trits_insert(trits, NUM_TRITS_SERIALIZED_TRANSACTION, transaction->attachment.trunk, NUM_TRITS_TRUNK, offset,
                    NUM_TRITS_TRUNK);
  offset += NUM_TRITS_TRUNK;
  flex_trits_insert(trits, NUM_TRITS_SERIALIZED_TRANSACTION, transaction->attachment.branch, NUM_TRITS_BRANCH, offset,
                    NUM_TRITS_BRANCH);
  offset += NUM_TRITS_BRANCH;
  flex_trits_insert(trits, NUM_TRITS_SERIALIZED_TRANSACTION, transaction->attachment.tag, NUM_TRITS_TAG, offset,
                    NUM_TRITS_TAG);
  offset += NUM_TRITS_TAG;
  _long_to_flex_trit(transaction->attachment.attachment_timestamp, partial);
  flex_trits_insert(trits, NUM_TRITS_SERIALIZED_TRANSACTION, partial, NUM_TRITS_ATTACHMENT_TIMESTAMP, offset,
                    NUM_TRITS_ATTACHMENT_TIMESTAMP);
  offset += NUM_TRITS_ATTACHMENT_TIMESTAMP;
  _long_to_flex_trit(transaction->attachment.attachment_timestamp_lower, partial);
  flex_trits_insert(trits, NUM_TRITS_SERIALIZED_TRANSACTION, partial, NUM_TRITS_ATTACHMENT_TIMESTAMP_LOWER, offset,
                    NUM_TRITS_ATTACHMENT_TIMESTAMP_LOWER);
  offset += NUM_TRITS_ATTACHMENT_TIMESTAMP_LOWER;
  _long_to_flex_trit(transaction->attachment.attachment_timestamp_upper, partial);
  flex_trits_insert(trits, NUM_TRITS_SERIALIZED_TRANSACTION, partial, NUM_TRITS_ATTACHMENT_TIMESTAMP_UPPER, offset,
                    NUM_TRITS_ATTACHMENT_TIMESTAMP_UPPER);
  offset += NUM_TRITS_ATTACHMENT_TIMESTAMP_UPPER;
  flex_trits_insert(trits, NUM_TRITS_SERIALIZED_TRANSACTION, transaction->attachment.nonce, NUM_TRITS_NONCE, offset,
                    NUM_TRITS_NONCE);
  offset += NUM_TRITS_NONCE;
  return offset;
}

/***********************************************************************************************************
 * Public interface
 ***********************************************************************************************************/

// Reset all transaction fields
void transaction_reset(iota_transaction_t *const transaction) {
  memset(transaction, 0, sizeof(iota_transaction_t));
  memset(transaction->essence.address, FLEX_TRIT_NULL_VALUE, sizeof(transaction->essence.address));
  memset(transaction->essence.obsolete_tag, FLEX_TRIT_NULL_VALUE, sizeof(transaction->essence.obsolete_tag));
  memset(transaction->essence.bundle, FLEX_TRIT_NULL_VALUE, sizeof(transaction->essence.bundle));
  memset(transaction->attachment.trunk, FLEX_TRIT_NULL_VALUE, sizeof(transaction->attachment.trunk));
  memset(transaction->attachment.branch, FLEX_TRIT_NULL_VALUE, sizeof(transaction->attachment.branch));
  memset(transaction->attachment.nonce, FLEX_TRIT_NULL_VALUE, sizeof(transaction->attachment.nonce));
  memset(transaction->attachment.tag, FLEX_TRIT_NULL_VALUE, sizeof(transaction->attachment.tag));
  memset(transaction->consensus.hash, FLEX_TRIT_NULL_VALUE, sizeof(transaction->consensus.hash));
  memset(transaction->data.signature_or_message, FLEX_TRIT_NULL_VALUE, sizeof(transaction->data.signature_or_message));
}

uint8_t transaction_weight_magnitude(iota_transaction_t const *const transaction) {
  uint8_t num_trailing_null_values = 0;
  uint8_t pos = FLEX_TRIT_SIZE_243;

  while (pos-- > 0 && transaction_hash(transaction)[pos] == FLEX_TRIT_NULL_VALUE) {
    num_trailing_null_values += NUM_TRITS_PER_FLEX_TRIT;
  }

  if (pos > 0) {
    trit_t one_trit_buffer[NUM_TRITS_PER_FLEX_TRIT];
    flex_trits_to_trits(one_trit_buffer, NUM_TRITS_PER_FLEX_TRIT, &transaction_hash(transaction)[pos],
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
iota_transaction_t *transaction_new(void) {
  iota_transaction_t *transaction;
  transaction = (iota_transaction_t *)malloc(sizeof(iota_transaction_t));
  if (!transaction) {
    // errno = IOTA_OUT_OF_MEMORY
  }
  transaction_reset(transaction);
  return transaction;
}

// Creates and returns a new transaction from serialized data
// Returns NULL if failed
iota_transaction_t *transaction_deserialize(flex_trit_t const *const trits, bool const compute_hash) {
  iota_transaction_t *transaction = transaction_new();

  if (!transaction) {
    // errno = IOTA_OUT_OF_MEMORY
    return NULL;
  }
  if (!transaction_deserialize_from_trits(transaction, trits, compute_hash)) {
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
flex_trit_t *transaction_serialize(iota_transaction_t const *const transaction) {
  size_t num_bytes = FLEX_TRIT_SIZE_8019;
  flex_trit_t *serialized_value = (flex_trit_t *)malloc(sizeof(flex_trit_t) * num_bytes);
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
size_t transaction_serialize_on_flex_trits(iota_transaction_t const *const transaction, flex_trit_t *const trits) {
  return transaction_serialize_to_flex_trits(transaction, trits);
}

/***********************************************************************************************************
 * Deserialization
 ***********************************************************************************************************/
// Fills up an existing transaction with the serialized data in trits - returns
// non 0 on success
size_t transaction_deserialize_from_trits(iota_transaction_t *const transaction, flex_trit_t const *const trits,
                                          bool const compute_hash) {
  return transaction_deserialize_trits(transaction, trits, compute_hash);
}

/***********************************************************************************************************
 * Destructor
 ***********************************************************************************************************/
// Free an existing transaction - compatible with free()
void transaction_free(iota_transaction_t *const transaction) { free(transaction); }

#ifdef DEBUG
void transaction_obj_dump(iota_transaction_t *tx_obj) {
  tryte_t trytes_81[NUM_TRYTES_HASH + 1];
  tryte_t trytes_27[NUM_TRYTES_TAG + 1];

  printf("==========Transaction Object==========\n");
  // address
  flex_trits_to_trytes(trytes_81, NUM_TRYTES_HASH, transaction_address(tx_obj), NUM_TRITS_HASH, NUM_TRITS_HASH);
  trytes_81[NUM_TRYTES_HASH] = '\0';
  printf("addr: %s\n", trytes_81);

  printf("value: %" PRId64 "\n", transaction_value(tx_obj));

  flex_trits_to_trytes(trytes_81, NUM_TRYTES_HASH, transaction_obsolete_tag(tx_obj), NUM_TRITS_HASH, NUM_TRITS_HASH);
  trytes_81[NUM_TRYTES_HASH] = '\0';
  printf("obsolete_tag: %s\n", trytes_81);

  printf("timestamp: %" PRId64 "\n", transaction_timestamp(tx_obj));
  printf("curr index: %" PRId64 " \nlast index: %" PRId64 "\n", transaction_current_index(tx_obj),
         transaction_last_index(tx_obj));

  flex_trits_to_trytes(trytes_81, NUM_TRYTES_HASH, transaction_bundle(tx_obj), NUM_TRITS_HASH, NUM_TRITS_HASH);
  trytes_81[NUM_TRYTES_HASH] = '\0';
  printf("bundle: %s\n", trytes_81);

  flex_trits_to_trytes(trytes_81, NUM_TRYTES_HASH, transaction_trunk(tx_obj), NUM_TRITS_HASH, NUM_TRITS_HASH);
  trytes_81[NUM_TRYTES_HASH] = '\0';
  printf("trunk: %s\n", trytes_81);

  flex_trits_to_trytes(trytes_81, NUM_TRYTES_HASH, transaction_branch(tx_obj), NUM_TRITS_HASH, NUM_TRITS_HASH);
  trytes_81[NUM_TRYTES_HASH] = '\0';
  printf("branch: %s\n", trytes_81);

  flex_trits_to_trytes(trytes_27, NUM_TRYTES_TAG, transaction_tag(tx_obj), NUM_TRITS_TAG, NUM_TRITS_TAG);
  trytes_27[NUM_TRYTES_TAG] = '\0';
  printf("tag: %s\n", trytes_27);

  printf("attachment_timestamp: %" PRId64 "\n", transaction_attachment_timestamp(tx_obj));
  printf("attachment_timestamp_lower: %" PRId64 "\n", transaction_attachment_timestamp_lower(tx_obj));
  printf("attachment_timestamp_upper: %" PRId64 "\n", transaction_attachment_timestamp_upper(tx_obj));

  flex_trits_to_trytes(trytes_27, NUM_TRYTES_TAG, transaction_nonce(tx_obj), NUM_TRITS_TAG, NUM_TRITS_TAG);
  trytes_27[NUM_TRYTES_TAG] = '\0';
  printf("nonce: %s\n", trytes_27);

  flex_trits_to_trytes(trytes_81, NUM_TRYTES_HASH, transaction_hash(tx_obj), NUM_TRITS_HASH, NUM_TRITS_HASH);
  trytes_81[NUM_TRYTES_HASH] = '\0';
  printf("hash: %s\n", trytes_81);
}
#endif
