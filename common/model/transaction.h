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

#include <assert.h>

#include "common/trinary/flex_trit.h"

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
#define NUM_TRITS_NONCE 81
#define NUM_TRITS_HASH 243

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
#define NUM_TRYTES_NONCE 27
#define NUM_TRYTES_HASH 81

/***********************************************************************************************************
 * Transaction data structure
 ***********************************************************************************************************/

typedef struct iota_transaction_fields_essence_s {
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
} iota_transaction_fields_essence_t;

typedef struct iota_transaction_fields_attachment_s {
  // 81 trytes = 243 trits
  flex_trit_t trunk[FLEX_TRIT_SIZE_243];
  // 81 trytes = 243 trits
  flex_trit_t branch[FLEX_TRIT_SIZE_243];
  // 9 trytes = 27 trits
  int64_t attachment_timestamp;
  // 9 trytes = 27 trits
  int64_t attachment_timestamp_lower;
  // 9 trytes = 27 trits
  int64_t attachment_timestamp_upper;
  // 27 trytes = 81 trits
  flex_trit_t nonce[FLEX_TRIT_SIZE_81];
  // 27 trytes = 81 trits
  flex_trit_t tag[FLEX_TRIT_SIZE_81];
} iota_transaction_fields_attachment_t;

typedef struct iota_transaction_fields_consensus_s {
  // 81 trytes = 243 trits
  flex_trit_t hash[FLEX_TRIT_SIZE_243];
} iota_transaction_fields_consensus_t;

typedef struct iota_transaction_fields_data_s {
  // 2187 trytes = 6561 trits
  flex_trit_t signature_or_message[FLEX_TRIT_SIZE_6561];
} iota_transaction_fields_data_t;

typedef struct iota_transaction_fields_metadata_s {
  uint64_t snapshot_index;
  bool solid;
  int64_t arrival_timestamp;
} iota_transaction_fields_metadata_t;

typedef struct iota_transaction_s {
  iota_transaction_fields_essence_t essence;
  iota_transaction_fields_attachment_t attachment;
  iota_transaction_fields_consensus_t consensus;
  iota_transaction_fields_data_t data;
  iota_transaction_fields_metadata_t metadata;
  int16_t loaded_columns_mask;
} iota_transaction_t;

typedef enum _field_mask {
  MASK_ESSENCE = (1u << 0),
  MASK_ATTACHMENT = (1u << 1),
  MASK_CONSENSUS = (1u << 2),
  MASK_DATA = (1u << 3),
  MASK_METADATA = (1u << 4),
  MASK_ALL_COLUMNS = MASK_ESSENCE | MASK_ATTACHMENT | MASK_CONSENSUS |
                     MASK_DATA | MASK_METADATA,
} field_mask_e;

/***********************************************************************************************************
 * Accessors
 ***********************************************************************************************************/

// Get the transaction signature
static inline flex_trit_t *transaction_signature(
    iota_transaction_t *const transaction) {
  assert(transaction->loaded_columns_mask & MASK_DATA);
  return transaction->data.signature_or_message;
}

// Set the transaction signature (copy argument)
static inline void transaction_set_signature(
    iota_transaction_t *const transaction, flex_trit_t const *const signature) {
  memcpy(transaction->data.signature_or_message, signature,
         sizeof(transaction->data.signature_or_message));
}

// Get the transaction message
static inline flex_trit_t *transaction_message(
    iota_transaction_t *const transaction) {
  assert(transaction->loaded_columns_mask & MASK_DATA);
  return transaction->data.signature_or_message;
}

// Set the transaction message (copy argument)
static inline void transaction_set_message(
    iota_transaction_t *const transaction, flex_trit_t const *const message) {
  memcpy(transaction->data.signature_or_message, message,
         sizeof(transaction->data.signature_or_message));
}

// Get the transaction address
static inline flex_trit_t *transaction_address(
    iota_transaction_t *const transaction) {
  assert(transaction->loaded_columns_mask & MASK_ESSENCE);
  return transaction->essence.address;
}

// Set the transaction address (copy argument)
static inline void transaction_set_address(
    iota_transaction_t *const transaction, flex_trit_t const *const address) {
  memcpy(transaction->essence.address, address,
         sizeof(transaction->essence.address));
}

// Get the transaction value
static inline int64_t transaction_value(iota_transaction_t *const transaction) {
  assert(transaction->loaded_columns_mask & MASK_ESSENCE);
  return transaction->essence.value;
}

// Set the transaction value
static inline void transaction_set_value(iota_transaction_t *const transaction,
                                         int64_t const value) {
  transaction->essence.value = value;
}

// Get the transaction obsolete tag
static inline flex_trit_t *transaction_obsolete_tag(
    iota_transaction_t *const transaction) {
  assert(transaction->loaded_columns_mask & MASK_ESSENCE);
  return transaction->essence.obsolete_tag;
}

// Set the transaction obsolete tag
static inline void transaction_set_obsolete_tag(
    iota_transaction_t *const transaction,
    flex_trit_t const *const obsolete_tag) {
  memcpy(transaction->essence.obsolete_tag, obsolete_tag,
         sizeof(transaction->essence.obsolete_tag));
}

// Get the transaction timestamp
static inline uint64_t transaction_timestamp(
    iota_transaction_t *const transaction) {
  assert(transaction->loaded_columns_mask & MASK_ESSENCE);
  return transaction->essence.timestamp;
}

// Set the transaction timestamp
static inline void transaction_set_timestamp(
    iota_transaction_t *const transaction, uint64_t const timestamp) {
  transaction->essence.timestamp = timestamp;
}

// Get the transaction current index
static inline int64_t transaction_current_index(
    iota_transaction_t *const transaction) {
  assert(transaction->loaded_columns_mask & MASK_ESSENCE);
  return transaction->essence.current_index;
}

// Set the transaction current index
static inline void transaction_set_current_index(
    iota_transaction_t *const transaction, int64_t const index) {
  transaction->essence.current_index = index;
}

// Get the transaction last index
static inline int64_t transaction_last_index(
    iota_transaction_t *const transaction) {
  return transaction->essence.last_index;
}

// Set the transaction last index
static inline void transaction_set_last_index(
    iota_transaction_t *const transaction, int64_t const index) {
  transaction->essence.last_index = index;
}

// Get the transaction bundle
static inline flex_trit_t *transaction_bundle(
    iota_transaction_t *const transaction) {
  assert(transaction->loaded_columns_mask & MASK_ESSENCE);
  return transaction->essence.bundle;
}

// Set the transaction bundle (copy argument)
static inline void transaction_set_bundle(iota_transaction_t *const transaction,
                                          flex_trit_t const *const bundle) {
  memcpy(transaction->essence.bundle, bundle,
         sizeof(transaction->essence.bundle));
}

// Get the transaction trunk
static inline flex_trit_t *transaction_trunk(
    iota_transaction_t *const transaction) {
  assert(transaction->loaded_columns_mask & MASK_ATTACHMENT);
  return transaction->attachment.trunk;
}

// Set the transaction trunk (copy argument)
static inline void transaction_set_trunk(iota_transaction_t *const transaction,
                                         flex_trit_t const *const trunk) {
  memcpy(transaction->attachment.trunk, trunk,
         sizeof(transaction->attachment.trunk));
}

// Get the transaction branch
static inline flex_trit_t *transaction_branch(
    iota_transaction_t *const transaction) {
  assert(transaction->loaded_columns_mask & MASK_ATTACHMENT);
  return transaction->attachment.branch;
}

// Set the transaction branch (copy argument)
static inline void transaction_set_branch(iota_transaction_t *const transaction,
                                          flex_trit_t const *const branch) {
  memcpy(transaction->attachment.branch, branch,
         sizeof(transaction->attachment.branch));
}

// Get the transaction tag
static inline flex_trit_t *transaction_tag(
    iota_transaction_t *const transaction) {
  assert(transaction->loaded_columns_mask & MASK_ATTACHMENT);
  return transaction->attachment.tag;
}

// Set the transaction tag (copy argument)
static inline void transaction_set_tag(iota_transaction_t *const transaction,
                                       flex_trit_t const *const tag) {
  memcpy(transaction->attachment.tag, tag, sizeof(transaction->attachment.tag));
}

// Get the transaction attachment timestamp
static inline int64_t transaction_attachment_timestamp(
    iota_transaction_t *const transaction) {
  assert(transaction->loaded_columns_mask & MASK_ATTACHMENT);
  return transaction->attachment.attachment_timestamp;
}

// Set the transaction attachment timestamp
static inline void transaction_set_attachment_timestamp(
    iota_transaction_t *const transaction, int64_t const timestamp) {
  transaction->attachment.attachment_timestamp = timestamp;
}

// Get the transaction attachment timestamp lower
static inline int64_t transaction_attachment_timestamp_lower(
    iota_transaction_t *const transaction) {
  assert(transaction->loaded_columns_mask & MASK_ATTACHMENT);
  return transaction->attachment.attachment_timestamp_lower;
}

// Set the transaction attachment timestamp lower
static inline void transaction_set_attachment_timestamp_lower(
    iota_transaction_t *const transaction, int64_t const timestamp) {
  transaction->attachment.attachment_timestamp_lower = timestamp;
}

// Get the transaction attachment timestamp upper
static inline int64_t transaction_attachment_timestamp_upper(
    iota_transaction_t *const transaction) {
  assert(transaction->loaded_columns_mask & MASK_ATTACHMENT);
  return transaction->attachment.attachment_timestamp_upper;
}

// Set the transaction attachment timestamp upper
static inline void transaction_set_attachment_timestamp_upper(
    iota_transaction_t *const transaction, int64_t const timestamp) {
  transaction->attachment.attachment_timestamp_upper = timestamp;
}

// Get the transaction nonce
static inline flex_trit_t *transaction_nonce(
    iota_transaction_t *const transaction) {
  assert(transaction->loaded_columns_mask & MASK_ATTACHMENT);
  return transaction->attachment.nonce;
}

// Set the transaction nonce (copy argument)
static inline void transaction_set_nonce(iota_transaction_t *const transaction,
                                         flex_trit_t const *const nonce) {
  memcpy(transaction->attachment.nonce, nonce,
         sizeof(transaction->attachment.nonce));
}

// Get the transaction hash
static inline flex_trit_t *transaction_hash(
    iota_transaction_t *const transaction) {
  assert(transaction->loaded_columns_mask & MASK_CONSENSUS);
  return transaction->consensus.hash;
}

// Set the transaction hash (copy argument)
static inline void transaction_set_hash(iota_transaction_t *const transaction,
                                        flex_trit_t const *const hash) {
  memcpy(transaction->consensus.hash, hash,
         sizeof(transaction->consensus.hash));
}

static inline uint64_t transaction_snapshot_index(
    iota_transaction_t *const transaction) {
  assert(transaction->loaded_columns_mask & MASK_METADATA);
  return transaction->metadata.snapshot_index;
}
// Set the transaction snapshot index
static inline void transaction_set_snapshot_index(
    iota_transaction_t *const transaction, uint64_t const snapshot_index) {
  transaction->metadata.snapshot_index = snapshot_index;
}

static inline bool transaction_solid(iota_transaction_t *const transaction) {
  assert(transaction->loaded_columns_mask & MASK_METADATA);
  return transaction->metadata.solid;
}
// Set the transaction solid state
static inline void transaction_set_solid(iota_transaction_t *const transaction,
                                         bool const state) {
  transaction->metadata.solid = state;
}

// Get the transaction arrival timestamp
static inline int64_t transaction_arrival_timestamp(
    iota_transaction_t *const transaction) {
  assert(transaction->loaded_columns_mask & MASK_METADATA);
  return transaction->metadata.arrival_timestamp;
}

// Set the transaction arrival timestamp
static inline void transaction_set_arrival_timestamp(
    iota_transaction_t *const transaction, int64_t const timestamp) {
  transaction->metadata.arrival_timestamp = timestamp;
}

/***********************************************************************************************************
 * Utility functions
 ***********************************************************************************************************/

extern uint8_t transaction_weight_magnitude(
    iota_transaction_t *const transaction);

/***********************************************************************************************************
 * Constructors
 ***********************************************************************************************************/
// Creates and returns a new transaction without data
iota_transaction_t *transaction_new(void);
// Creates and returns a new transaction from serialized data - NULL if failed
iota_transaction_t *transaction_deserialize(flex_trit_t const *const trits,
                                            bool const compute_hash);
/***********************************************************************************************************
 * Serialization
 ***********************************************************************************************************/
// Returns the serialized data from an existing transaction - NULL if failed
flex_trit_t *transaction_serialize(iota_transaction_t const *const transaction);
// Places the serialized data from an existing transaction in pre-allocated
// string - returns non 0 on success
size_t transaction_serialize_on_flex_trits(
    iota_transaction_t const *const transaction, flex_trit_t *const trits);
/***********************************************************************************************************
 * Deserialization
 ***********************************************************************************************************/
// Fills up an existing transaction with the serialized data in trits - returns
// non 0 on success
size_t transaction_deserialize_from_trits(iota_transaction_t *const transaction,
                                          flex_trit_t const *const trits,
                                          bool compute_hash);
/***********************************************************************************************************
 * Destructor
 ***********************************************************************************************************/
// Free an existing transaction - compatible with free()
void transaction_free(iota_transaction_t *const transaction);

#endif  // __COMMON_MODEL_TRANSACTION_H_
#ifdef __cplusplus
}
#endif
