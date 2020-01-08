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
#include "utarray.h"

#define NUM_TRITS_SERIALIZED_TRANSACTION 8019
#define NUM_TRITS_SIGNATURE 6561
#define NUM_TRITS_MESSAGE 6561
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
#define NUM_TRYTES_MESSAGE 2187
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

#define NUM_FLEX_TRITS_SERIALIZED_TRANSACTION FLEX_TRIT_SIZE_8019
#define NUM_FLEX_TRITS_SIGNATURE FLEX_TRIT_SIZE_6561
#define NUM_FLEX_TRITS_MESSAGE FLEX_TRIT_SIZE_6561
#define NUM_FLEX_TRITS_ADDRESS FLEX_TRIT_SIZE_243
#define NUM_FLEX_TRITS_VALUE FLEX_TRIT_SIZE_81
#define NUM_FLEX_TRITS_OBSOLETE_TAG FLEX_TRIT_SIZE_81
#define NUM_FLEX_TRITS_TIMESTAMP FLEX_TRIT_SIZE_27
#define NUM_FLEX_TRITS_CURRENT_INDEX FLEX_TRIT_SIZE_27
#define NUM_FLEX_TRITS_LAST_INDEX FLEX_TRIT_SIZE_27
#define NUM_FLEX_TRITS_BUNDLE FLEX_TRIT_SIZE_243
#define NUM_FLEX_TRITS_TRUNK FLEX_TRIT_SIZE_243
#define NUM_FLEX_TRITS_BRANCH FLEX_TRIT_SIZE_243
#define NUM_FLEX_TRITS_TAG FLEX_TRIT_SIZE_81
#define NUM_FLEX_TRITS_ATTACHMENT_TIMESTAMP FLEX_TRIT_SIZE_27
#define NUM_FLEX_TRITS_ATTACHMENT_TIMESTAMP_LOWER FLEX_TRIT_SIZE_27
#define NUM_FLEX_TRITS_ATTACHMENT_TIMESTAMP_UPPER FLEX_TRIT_SIZE_27
#define NUM_FLEX_TRITS_NONCE FLEX_TRIT_SIZE_81
#define NUM_FLEX_TRITS_HASH FLEX_TRIT_SIZE_243

// bundle essence = 486 trits
#define NUM_TRITS_ESSENCE                                                                                         \
  (NUM_TRITS_ADDRESS + NUM_TRITS_VALUE + NUM_TRITS_OBSOLETE_TAG + NUM_TRITS_TIMESTAMP + NUM_TRITS_CURRENT_INDEX + \
   NUM_TRITS_LAST_INDEX)

/***********************************************************************************************************
 * Transaction data structure
 ***********************************************************************************************************/

typedef struct iota_transaction_fields_essence_s {
  // 81 trytes = 243 trits
  flex_trit_t address[NUM_FLEX_TRITS_ADDRESS];
  // 27 trytes = 81 trits
  int64_t value;
  // 27 trytes = 81 trits
  flex_trit_t obsolete_tag[NUM_FLEX_TRITS_OBSOLETE_TAG];
  // 9 trytes = 27 trits
  uint64_t timestamp;
  // 9 trytes = 27 trits
  uint64_t current_index;
  // 9 trytes = 27 trits
  uint64_t last_index;
  // 81 trytes = 243 trits
  flex_trit_t bundle[NUM_FLEX_TRITS_BUNDLE];
} iota_transaction_fields_essence_t;

typedef struct iota_transaction_fields_attachment_s {
  // 81 trytes = 243 trits
  flex_trit_t trunk[NUM_FLEX_TRITS_TRUNK];
  // 81 trytes = 243 trits
  flex_trit_t branch[NUM_FLEX_TRITS_BRANCH];
  // 9 trytes = 27 trits
  uint64_t attachment_timestamp;
  // 9 trytes = 27 trits
  uint64_t attachment_timestamp_lower;
  // 9 trytes = 27 trits
  uint64_t attachment_timestamp_upper;
  // 27 trytes = 81 trits
  flex_trit_t nonce[NUM_FLEX_TRITS_NONCE];
  // 27 trytes = 81 trits
  flex_trit_t tag[NUM_FLEX_TRITS_TAG];
} iota_transaction_fields_attachment_t;

typedef struct iota_transaction_fields_consensus_s {
  // 81 trytes = 243 trits
  flex_trit_t hash[NUM_FLEX_TRITS_HASH];
} iota_transaction_fields_consensus_t;

typedef struct iota_transaction_fields_data_s {
  // 2187 trytes = 6561 trits
  flex_trit_t signature_or_message[NUM_FLEX_TRITS_MESSAGE];
} iota_transaction_fields_data_t;

typedef struct iota_transaction_fields_metadata_s {
  uint64_t snapshot_index;
  bool solid;
  uint8_t validity;
  uint64_t arrival_timestamp;
} iota_transaction_fields_metadata_t;

typedef struct field_mask_s {
  uint8_t essence;
  uint8_t attachment;
  uint8_t consensus;
  uint8_t data;
  uint8_t metadata;
} field_mask_t;

typedef struct iota_transaction_s {
  iota_transaction_fields_essence_t essence;
  iota_transaction_fields_attachment_t attachment;
  iota_transaction_fields_consensus_t consensus;
  iota_transaction_fields_data_t data;
  iota_transaction_fields_metadata_t metadata;
  field_mask_t loaded_columns_mask;
} iota_transaction_t;

typedef enum field_mask_essence_e {
  MASK_ESSENCE_ADDRESS = (1u << 0),
  MASK_ESSENCE_VALUE = (1u << 1),
  MASK_ESSENCE_OBSOLETE_TAG = (1u << 2),
  MASK_ESSENCE_TIMESTAMP = (1u << 3),
  MASK_ESSENCE_CURRENT_INDEX = (1u << 4),
  MASK_ESSENCE_LAST_INDEX = (1u << 5),
  MASK_ESSENCE_BUNDLE = (1u << 6),
  MASK_ESSENCE_ALL = MASK_ESSENCE_ADDRESS | MASK_ESSENCE_VALUE | MASK_ESSENCE_OBSOLETE_TAG | MASK_ESSENCE_TIMESTAMP |
                     MASK_ESSENCE_CURRENT_INDEX | MASK_ESSENCE_LAST_INDEX | MASK_ESSENCE_BUNDLE
} field_mask_essence_t;

typedef enum field_mask_attachment_e {
  MASK_ATTACHMENT_TRUNK = (1u << 0),
  MASK_ATTACHMENT_BRANCH = (1u << 1),
  MASK_ATTACHMENT_TIMESTAMP = (1u << 2),
  MASK_ATTACHMENT_TIMESTAMP_LOWER = (1u << 3),
  MASK_ATTACHMENT_TIMESTAMP_UPPER = (1u << 4),
  MASK_ATTACHMENT_NONCE = (1u << 5),
  MASK_ATTACHMENT_TAG = (1u << 6),
  MASK_ATTACHMENT_ALL = MASK_ATTACHMENT_TRUNK | MASK_ATTACHMENT_BRANCH | MASK_ATTACHMENT_TIMESTAMP |
                        MASK_ATTACHMENT_TIMESTAMP_LOWER | MASK_ATTACHMENT_TIMESTAMP_UPPER | MASK_ATTACHMENT_NONCE |
                        MASK_ATTACHMENT_TAG
} field_mask_attachment_t;

typedef enum field_mask_consensus_e {
  MASK_CONSENSUS_HASH = (1u << 0),
  MASK_CONSENSUS_ALL = MASK_CONSENSUS_HASH
} field_mask_consensus_t;

typedef enum field_mask_data_e {
  MASK_DATA_SIG_OR_MSG = (1u << 0),
  MASK_DATA_ALL = MASK_DATA_SIG_OR_MSG
} field_mask_data_t;

typedef enum field_mask_metadata_e {
  MASK_METADATA_SNAPSHOT_INDEX = (1u << 0),
  MASK_METADATA_SOLID = (1u << 1),
  MASK_METADATA_VALIDITY = (1u << 2),
  MASK_METADATA_ARRIVAL_TIMESTAMP = (1u << 3),
  MASK_METADATA_ALL =
      MASK_METADATA_SNAPSHOT_INDEX | MASK_METADATA_SOLID | MASK_METADATA_VALIDITY | MASK_METADATA_ARRIVAL_TIMESTAMP
} field_mask_metadata_t;

/***********************************************************************************************************
 * Accessors
 ***********************************************************************************************************/

// Get the transaction signature
static inline flex_trit_t *transaction_signature(iota_transaction_t const *const transaction) {
  assert(transaction->loaded_columns_mask.data & MASK_DATA_SIG_OR_MSG);
  return (flex_trit_t *)transaction->data.signature_or_message;
}

// Set the transaction signature (copy argument)
static inline void transaction_set_signature(iota_transaction_t *const transaction,
                                             flex_trit_t const *const signature) {
  memcpy(transaction->data.signature_or_message, signature, sizeof(transaction->data.signature_or_message));
  transaction->loaded_columns_mask.data |= MASK_DATA_SIG_OR_MSG;
}

// Get the transaction message
static inline flex_trit_t *transaction_message(iota_transaction_t const *const transaction) {
  assert(transaction->loaded_columns_mask.data & MASK_DATA_SIG_OR_MSG);
  return (flex_trit_t *)transaction->data.signature_or_message;
}

// Set the transaction message (copy argument)
static inline void transaction_set_message(iota_transaction_t *const transaction, flex_trit_t const *const message) {
  memcpy(transaction->data.signature_or_message, message, sizeof(transaction->data.signature_or_message));
  transaction->loaded_columns_mask.data |= MASK_DATA_SIG_OR_MSG;
}

// Get the transaction address
static inline flex_trit_t *transaction_address(iota_transaction_t const *const transaction) {
  assert(transaction->loaded_columns_mask.essence & MASK_ESSENCE_ADDRESS);
  return (flex_trit_t *)transaction->essence.address;
}

// Set the transaction address (copy argument)
static inline void transaction_set_address(iota_transaction_t *const transaction, flex_trit_t const *const address) {
  memcpy(transaction->essence.address, address, sizeof(transaction->essence.address));
  transaction->loaded_columns_mask.essence |= MASK_ESSENCE_ADDRESS;
}

// Get the transaction value
static inline int64_t transaction_value(iota_transaction_t const *const transaction) {
  assert(transaction->loaded_columns_mask.essence & MASK_ESSENCE_VALUE);
  return transaction->essence.value;
}

// Set the transaction value
static inline void transaction_set_value(iota_transaction_t *const transaction, int64_t const value) {
  transaction->essence.value = value;
  transaction->loaded_columns_mask.essence |= MASK_ESSENCE_VALUE;
}

// Get the transaction obsolete tag
static inline flex_trit_t *transaction_obsolete_tag(iota_transaction_t const *const transaction) {
  assert(transaction->loaded_columns_mask.essence & MASK_ESSENCE_OBSOLETE_TAG);
  return (flex_trit_t *)transaction->essence.obsolete_tag;
}

// Set the transaction obsolete tag
static inline void transaction_set_obsolete_tag(iota_transaction_t *const transaction,
                                                flex_trit_t const *const obsolete_tag) {
  memcpy(transaction->essence.obsolete_tag, obsolete_tag, sizeof(transaction->essence.obsolete_tag));
  transaction->loaded_columns_mask.essence |= MASK_ESSENCE_OBSOLETE_TAG;
}

// Get the transaction timestamp
static inline uint64_t transaction_timestamp(iota_transaction_t const *const transaction) {
  assert(transaction->loaded_columns_mask.essence & MASK_ESSENCE_TIMESTAMP);
  return transaction->essence.timestamp;
}

// Set the transaction timestamp
static inline void transaction_set_timestamp(iota_transaction_t *const transaction, uint64_t const timestamp) {
  transaction->essence.timestamp = timestamp;
  transaction->loaded_columns_mask.essence |= MASK_ESSENCE_TIMESTAMP;
}

// Get the transaction current index
static inline uint64_t transaction_current_index(iota_transaction_t const *const transaction) {
  assert(transaction->loaded_columns_mask.essence & MASK_ESSENCE_CURRENT_INDEX);
  return transaction->essence.current_index;
}

// Set the transaction current index
static inline void transaction_set_current_index(iota_transaction_t *const transaction, uint64_t const index) {
  transaction->essence.current_index = index;
  transaction->loaded_columns_mask.essence |= MASK_ESSENCE_CURRENT_INDEX;
}

// Get the transaction last index
static inline uint64_t transaction_last_index(iota_transaction_t const *const transaction) {
  assert(transaction->loaded_columns_mask.essence & MASK_ESSENCE_LAST_INDEX);
  return transaction->essence.last_index;
}

// Set the transaction last index
static inline void transaction_set_last_index(iota_transaction_t *const transaction, uint64_t const index) {
  transaction->essence.last_index = index;
  transaction->loaded_columns_mask.essence |= MASK_ESSENCE_LAST_INDEX;
}

// Get the transaction bundle
static inline flex_trit_t *transaction_bundle(iota_transaction_t const *const transaction) {
  assert(transaction->loaded_columns_mask.essence & MASK_ESSENCE_BUNDLE);
  return (flex_trit_t *)transaction->essence.bundle;
}

// Set the transaction bundle (copy argument)
static inline void transaction_set_bundle(iota_transaction_t *const transaction, flex_trit_t const *const bundle) {
  memcpy(transaction->essence.bundle, bundle, sizeof(transaction->essence.bundle));
  transaction->loaded_columns_mask.essence |= MASK_ESSENCE_BUNDLE;
}

// Get the transaction trunk
static inline flex_trit_t *transaction_trunk(iota_transaction_t const *const transaction) {
  assert(transaction->loaded_columns_mask.attachment & MASK_ATTACHMENT_TRUNK);
  return (flex_trit_t *)transaction->attachment.trunk;
}

// Set the transaction trunk (copy argument)
static inline void transaction_set_trunk(iota_transaction_t *const transaction, flex_trit_t const *const trunk) {
  memcpy(transaction->attachment.trunk, trunk, sizeof(transaction->attachment.trunk));
  transaction->loaded_columns_mask.attachment |= MASK_ATTACHMENT_TRUNK;
}

// Get the transaction branch
static inline flex_trit_t *transaction_branch(iota_transaction_t const *const transaction) {
  assert(transaction->loaded_columns_mask.attachment & MASK_ATTACHMENT_BRANCH);
  return (flex_trit_t *)transaction->attachment.branch;
}

// Set the transaction branch (copy argument)
static inline void transaction_set_branch(iota_transaction_t *const transaction, flex_trit_t const *const branch) {
  memcpy(transaction->attachment.branch, branch, sizeof(transaction->attachment.branch));
  transaction->loaded_columns_mask.attachment |= MASK_ATTACHMENT_BRANCH;
}

// Get the transaction tag
static inline flex_trit_t *transaction_tag(iota_transaction_t const *const transaction) {
  assert(transaction->loaded_columns_mask.attachment & MASK_ATTACHMENT_TAG);
  return (flex_trit_t *)transaction->attachment.tag;
}

// Set the transaction tag (copy argument)
static inline void transaction_set_tag(iota_transaction_t *const transaction, flex_trit_t const *const tag) {
  memcpy(transaction->attachment.tag, tag, sizeof(transaction->attachment.tag));
  transaction->loaded_columns_mask.attachment |= MASK_ATTACHMENT_TAG;
}

// Get the transaction attachment timestamp
static inline uint64_t transaction_attachment_timestamp(iota_transaction_t const *const transaction) {
  assert(transaction->loaded_columns_mask.attachment & MASK_ATTACHMENT_TIMESTAMP);
  return transaction->attachment.attachment_timestamp;
}

// Set the transaction attachment timestamp
static inline void transaction_set_attachment_timestamp(iota_transaction_t *const transaction,
                                                        uint64_t const timestamp) {
  transaction->attachment.attachment_timestamp = timestamp;
  transaction->loaded_columns_mask.attachment |= MASK_ATTACHMENT_TIMESTAMP;
}

// Get the transaction attachment timestamp lower
static inline uint64_t transaction_attachment_timestamp_lower(iota_transaction_t const *const transaction) {
  assert(transaction->loaded_columns_mask.attachment & MASK_ATTACHMENT_TIMESTAMP_LOWER);
  return transaction->attachment.attachment_timestamp_lower;
}

// Set the transaction attachment timestamp lower
static inline void transaction_set_attachment_timestamp_lower(iota_transaction_t *const transaction,
                                                              uint64_t const timestamp) {
  transaction->attachment.attachment_timestamp_lower = timestamp;
  transaction->loaded_columns_mask.attachment |= MASK_ATTACHMENT_TIMESTAMP_LOWER;
}

// Get the transaction attachment timestamp upper
static inline uint64_t transaction_attachment_timestamp_upper(iota_transaction_t const *const transaction) {
  assert(transaction->loaded_columns_mask.attachment & MASK_ATTACHMENT_TIMESTAMP_UPPER);
  return transaction->attachment.attachment_timestamp_upper;
}

// Set the transaction attachment timestamp upper
static inline void transaction_set_attachment_timestamp_upper(iota_transaction_t *const transaction,
                                                              uint64_t const timestamp) {
  transaction->attachment.attachment_timestamp_upper = timestamp;
  transaction->loaded_columns_mask.attachment |= MASK_ATTACHMENT_TIMESTAMP_UPPER;
}

// Get the transaction nonce
static inline flex_trit_t *transaction_nonce(iota_transaction_t const *const transaction) {
  assert(transaction->loaded_columns_mask.attachment & MASK_ATTACHMENT_NONCE);
  return (flex_trit_t *)transaction->attachment.nonce;
}

// Set the transaction nonce (copy argument)
static inline void transaction_set_nonce(iota_transaction_t *const transaction, flex_trit_t const *const nonce) {
  memcpy(transaction->attachment.nonce, nonce, sizeof(transaction->attachment.nonce));
  transaction->loaded_columns_mask.attachment |= MASK_ATTACHMENT_NONCE;
}

// Get the transaction hash
static inline flex_trit_t *transaction_hash(iota_transaction_t const *const transaction) {
  assert(transaction->loaded_columns_mask.consensus & MASK_CONSENSUS_HASH);
  return (flex_trit_t *)transaction->consensus.hash;
}

// Set the transaction hash (copy argument)
static inline void transaction_set_hash(iota_transaction_t *const transaction, flex_trit_t const *const hash) {
  memcpy(transaction->consensus.hash, hash, sizeof(transaction->consensus.hash));
  transaction->loaded_columns_mask.consensus |= MASK_CONSENSUS_HASH;
}

static inline uint64_t transaction_snapshot_index(iota_transaction_t const *const transaction) {
  assert(transaction->loaded_columns_mask.metadata & MASK_METADATA_SNAPSHOT_INDEX);
  return transaction->metadata.snapshot_index;
}
// Set the transaction snapshot index
static inline void transaction_set_snapshot_index(iota_transaction_t *const transaction,
                                                  uint64_t const snapshot_index) {
  transaction->metadata.snapshot_index = snapshot_index;
  transaction->loaded_columns_mask.metadata |= MASK_METADATA_SNAPSHOT_INDEX;
}

static inline bool transaction_solid(iota_transaction_t const *const transaction) {
  assert(transaction->loaded_columns_mask.metadata & MASK_METADATA_SOLID);
  return transaction->metadata.solid;
}
// Set the transaction solid state
static inline void transaction_set_solid(iota_transaction_t *const transaction, bool const state) {
  transaction->metadata.solid = state;
  transaction->loaded_columns_mask.metadata |= MASK_METADATA_SOLID;
}

// Get the transaction validity
static inline uint8_t transaction_validity(iota_transaction_t const *const transaction) {
  assert(transaction->loaded_columns_mask.metadata & MASK_METADATA_VALIDITY);
  return transaction->metadata.validity;
}
// Set the transaction validity
static inline void transaction_set_validity(iota_transaction_t *const transaction, uint8_t const validity) {
  transaction->metadata.validity = validity;
  transaction->loaded_columns_mask.metadata |= MASK_METADATA_VALIDITY;
}

// Get the transaction arrival timestamp
static inline uint64_t transaction_arrival_timestamp(iota_transaction_t const *const transaction) {
  assert(transaction->loaded_columns_mask.metadata & MASK_METADATA_ARRIVAL_TIMESTAMP);
  return transaction->metadata.arrival_timestamp;
}

// Set the transaction arrival timestamp
static inline void transaction_set_arrival_timestamp(iota_transaction_t *const transaction, uint64_t const timestamp) {
  transaction->metadata.arrival_timestamp = timestamp;
  transaction->loaded_columns_mask.metadata |= MASK_METADATA_ARRIVAL_TIMESTAMP;
}

/***********************************************************************************************************
 * Utility functions
 ***********************************************************************************************************/

void transaction_reset(iota_transaction_t *const transaction);
#ifdef DEBUG
void transaction_obj_dump(iota_transaction_t *tx_obj);
#endif

uint8_t transaction_weight_magnitude(iota_transaction_t const *const transaction);

/***********************************************************************************************************
 * Constructors
 ***********************************************************************************************************/
// Creates and returns a new transaction without data
iota_transaction_t *transaction_new(void);
// Creates and returns a new transaction from serialized data - NULL if failed
iota_transaction_t *transaction_deserialize(flex_trit_t const *const trits, bool const compute_hash);
/***********************************************************************************************************
 * Serialization
 ***********************************************************************************************************/
// Returns the serialized data from an existing transaction - NULL if failed
flex_trit_t *transaction_serialize(iota_transaction_t const *const transaction);
// Places the serialized data from an existing transaction in pre-allocated
// string - returns non 0 on success
size_t transaction_serialize_on_flex_trits(iota_transaction_t const *const transaction, flex_trit_t *const trits);
/***********************************************************************************************************
 * Deserialization
 ***********************************************************************************************************/
// Fills up an existing transaction with the serialized data in trits - returns
// non 0 on success
size_t transaction_deserialize_from_trits(iota_transaction_t *const transaction, flex_trit_t const *const trits,
                                          bool const compute_hash);
/***********************************************************************************************************
 * Destructor
 ***********************************************************************************************************/
// Free an existing transaction - compatible with free()
void transaction_free(iota_transaction_t *const transaction);

typedef UT_array transaction_array_t;
static UT_icd const ut_transactions_icd = {sizeof(iota_transaction_t), NULL, NULL, NULL};
static inline transaction_array_t *transaction_array_new() {
  transaction_array_t *txs = NULL;
  utarray_new(txs, &ut_transactions_icd);
  return txs;
}
static inline void transaction_array_push_back(transaction_array_t *txs, iota_transaction_t const *const tx) {
  utarray_push_back(txs, tx);
}
static inline size_t transaction_array_len(transaction_array_t *txs) { return utarray_len(txs); }
static inline void transaction_array_free(transaction_array_t *txs) { utarray_free(txs); }
static inline iota_transaction_t *transaction_array_at(transaction_array_t *txs, size_t index) {
  // return NULL if not found.
  return (iota_transaction_t *)utarray_eltptr(txs, index);
}

#define TX_OBJS_FOREACH(txs, tx) \
  for (tx = (iota_transaction_t *)utarray_front(txs); tx != NULL; tx = (iota_transaction_t *)utarray_next(txs, tx))

#endif  // __COMMON_MODEL_TRANSACTION_H_
#ifdef __cplusplus
}
#endif
