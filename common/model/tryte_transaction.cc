/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#include "common/model/tryte_transaction.h"

namespace iota {
namespace model {
TryteTransaction::TryteTransaction() { iota_transaction = transaction_new(); }

TryteTransaction::TryteTransaction(const std::string &trytes) {
  iota_transaction = transaction_new();
  size_t to_len = trytes.size() * 3;
  size_t flex_len = NUM_FLEX_TRITS_FOR_TRITS(to_len);
  flex_trit_t trits[flex_len];
  flex_trits_from_trytes(trits, to_len, (const tryte_t *)trytes.data(),
                         trytes.size(), trytes.size());
  transaction_deserialize_from_trits(iota_transaction, trits);
}

TryteTransaction::~TryteTransaction() { transaction_free(iota_transaction); }

std::string TryteTransaction::signature(void) {
  auto trits = transaction_signature(iota_transaction);
  std::string trytes(NUM_TRYTES_SIGNATURE, '9');
  flex_trits_to_trytes((tryte_t *)trytes.data(), NUM_TRYTES_SIGNATURE, trits,
                       NUM_TRITS_SIGNATURE, NUM_TRITS_SIGNATURE);
  return trytes;
}

// Set the transaction signature (assign)
void TryteTransaction::setSignature(const std::string &trytes) {
  size_t flex_len = NUM_FLEX_TRITS_FOR_TRITS(NUM_TRITS_SIGNATURE);
  flex_trit_t trits[flex_len];
  flex_trits_from_trytes(trits, NUM_TRITS_SIGNATURE,
                         (const tryte_t *)trytes.data(), NUM_TRYTES_SIGNATURE,
                         NUM_TRYTES_SIGNATURE);
  transaction_set_signature(iota_transaction, trits);
}

// Get the transaction message
std::string TryteTransaction::message(void) {
  auto trits = transaction_message(iota_transaction);
  std::string trytes(NUM_TRYTES_SIGNATURE, '9');
  flex_trits_to_trytes((tryte_t *)trytes.data(), NUM_TRYTES_SIGNATURE, trits,
                       NUM_TRITS_SIGNATURE, NUM_TRITS_SIGNATURE);
  return trytes;
}

// Set the transaction message (assign)
void TryteTransaction::setMessage(const std::string &trytes) {
  size_t flex_len = NUM_FLEX_TRITS_FOR_TRITS(NUM_TRITS_SIGNATURE);
  flex_trit_t trits[flex_len];
  flex_trits_from_trytes(trits, NUM_TRITS_SIGNATURE,
                         (const tryte_t *)trytes.data(), NUM_TRYTES_SIGNATURE,
                         NUM_TRYTES_SIGNATURE);
  transaction_set_message(iota_transaction, trits);
}

// Get the transaction address
std::string TryteTransaction::address(void) {
  auto trits = transaction_address(iota_transaction);
  std::string trytes(NUM_TRYTES_ADDRESS, '9');
  flex_trits_to_trytes((tryte_t *)trytes.data(), NUM_TRYTES_ADDRESS, trits,
                       NUM_TRITS_ADDRESS, NUM_TRITS_ADDRESS);
  return trytes;
}

// Set the transaction address (assign)
void TryteTransaction::setAddress(const std::string &trytes) {
  size_t flex_len = NUM_FLEX_TRITS_FOR_TRITS(NUM_TRITS_ADDRESS);
  flex_trit_t trits[flex_len];
  flex_trits_from_trytes(trits, NUM_TRITS_ADDRESS,
                         (const tryte_t *)trytes.data(), NUM_TRYTES_ADDRESS,
                         NUM_TRYTES_ADDRESS);
  transaction_set_address(iota_transaction, trits);
}

// Get the transaction value
int64_t TryteTransaction::value(void) {
  return transaction_value(iota_transaction);
}

// Set the transaction value
void TryteTransaction::setValue(int64_t value) {
  transaction_set_value(iota_transaction, value);
}

// Get the transaction obsolete tag
std::string TryteTransaction::obsoleteTag(void) {
  auto trits = transaction_obsolete_tag(iota_transaction);
  std::string trytes(NUM_TRYTES_OBSOLETE_TAG, '9');
  flex_trits_to_trytes((tryte_t *)trytes.data(), NUM_TRYTES_OBSOLETE_TAG, trits,
                       NUM_TRITS_OBSOLETE_TAG, NUM_TRITS_OBSOLETE_TAG);
  return trytes;
}

// Set the transaction obsolete tag
void TryteTransaction::setObsoleteTag(const std::string &trytes) {
  size_t flex_len = NUM_FLEX_TRITS_FOR_TRITS(NUM_TRITS_OBSOLETE_TAG);
  flex_trit_t trits[flex_len];
  flex_trits_from_trytes(trits, NUM_TRITS_OBSOLETE_TAG,
                         (const tryte_t *)trytes.data(),
                         NUM_TRYTES_OBSOLETE_TAG, NUM_TRYTES_OBSOLETE_TAG);
  transaction_set_obsolete_tag(iota_transaction, trits);
}

// Get the transaction timestamp
uint64_t TryteTransaction::timestamp(void) {
  return transaction_timestamp(iota_transaction);
}

// Set the transaction timestamp
void TryteTransaction::setTimestamp(uint64_t timestamp) {
  transaction_set_timestamp(iota_transaction, timestamp);
}

// Get the transaction current index
int64_t TryteTransaction::currentIndex(void) {
  return transaction_current_index(iota_transaction);
}

// Set the transaction current index
void TryteTransaction::setCurrentIndex(int64_t index) {
  transaction_set_current_index(iota_transaction, index);
}

// Get the transaction last index
int64_t TryteTransaction::lastIndex(void) {
  return transaction_last_index(iota_transaction);
}

// Set the transaction last index
void TryteTransaction::setLastIndex(int64_t last_index) {
  transaction_set_last_index(iota_transaction, last_index);
}

// Get the transaction bundle
std::string TryteTransaction::bundle(void) {
  auto trits = transaction_bundle(iota_transaction);
  std::string trytes(NUM_TRYTES_BUNDLE, '9');
  flex_trits_to_trytes((tryte_t *)trytes.data(), NUM_TRYTES_BUNDLE, trits,
                       NUM_TRITS_BUNDLE, NUM_TRITS_BUNDLE);
  return trytes;
}

// Set the transaction bundle (assign)
void TryteTransaction::setBundle(const std::string &trytes) {
  size_t flex_len = NUM_FLEX_TRITS_FOR_TRITS(NUM_TRITS_BUNDLE);
  flex_trit_t trits[flex_len];
  flex_trits_from_trytes(trits, NUM_TRITS_BUNDLE,
                         (const tryte_t *)trytes.data(), NUM_TRYTES_BUNDLE,
                         NUM_TRYTES_BUNDLE);
  transaction_set_bundle(iota_transaction, trits);
}

// Get the transaction trunk
std::string TryteTransaction::trunk(void) {
  auto trits = transaction_trunk(iota_transaction);
  std::string trytes(NUM_TRYTES_TRUNK, '9');
  flex_trits_to_trytes((tryte_t *)trytes.data(), NUM_TRYTES_TRUNK, trits,
                       NUM_TRITS_TRUNK, NUM_TRITS_TRUNK);
  return trytes;
}

// Set the transaction trunk (assign)
void TryteTransaction::setTrunk(const std::string &trytes) {
  size_t flex_len = NUM_FLEX_TRITS_FOR_TRITS(NUM_TRITS_TRUNK);
  flex_trit_t trits[flex_len];
  flex_trits_from_trytes(trits, NUM_TRITS_TRUNK, (const tryte_t *)trytes.data(),
                         NUM_TRYTES_TRUNK, NUM_TRYTES_TRUNK);
  transaction_set_trunk(iota_transaction, trits);
}

// Get the transaction branch
std::string TryteTransaction::branch(void) {
  auto trits = transaction_branch(iota_transaction);
  std::string trytes(NUM_TRYTES_BRANCH, '9');
  flex_trits_to_trytes((tryte_t *)trytes.data(), NUM_TRYTES_BRANCH, trits,
                       NUM_TRITS_BRANCH, NUM_TRITS_BRANCH);
  return trytes;
}

// Set the transaction branch (assign)
void TryteTransaction::setBranch(const std::string &trytes) {
  size_t flex_len = NUM_FLEX_TRITS_FOR_TRITS(NUM_TRITS_BRANCH);
  flex_trit_t trits[flex_len];
  flex_trits_from_trytes(trits, NUM_TRITS_BRANCH,
                         (const tryte_t *)trytes.data(), NUM_TRYTES_BRANCH,
                         NUM_TRYTES_BRANCH);
  transaction_set_branch(iota_transaction, trits);
}

// Get the transaction tag
std::string TryteTransaction::tag(void) {
  auto trits = transaction_tag(iota_transaction);
  std::string trytes(NUM_TRYTES_TAG, '9');
  flex_trits_to_trytes((tryte_t *)trytes.data(), NUM_TRYTES_TAG, trits,
                       NUM_TRITS_TAG, NUM_TRITS_TAG);
  return trytes;
}

// Set the transaction tag (assign)
void TryteTransaction::setTag(const std::string &trytes) {
  size_t flex_len = NUM_FLEX_TRITS_FOR_TRITS(NUM_TRITS_TAG);
  flex_trit_t trits[flex_len];
  flex_trits_from_trytes(trits, NUM_TRITS_TAG, (const tryte_t *)trytes.data(),
                         NUM_TRYTES_TAG, NUM_TRYTES_TAG);
  transaction_set_tag(iota_transaction, trits);
}

// Get the transaction attachment timestamp
int64_t TryteTransaction::attachmentTimestamp(void) {
  return transaction_attachment_timestamp(iota_transaction);
}

// Set the transaction attachment timestamp
void TryteTransaction::setAttachmentTimestamp(int64_t timestamp) {
  transaction_set_attachment_timestamp(iota_transaction, timestamp);
}

// Get the transaction attachment timestamp lower
int64_t TryteTransaction::attachmentTimestampLower(void) {
  return transaction_attachment_timestamp_lower(iota_transaction);
}

// Set the transaction attachment timestamp lower
void TryteTransaction::setAttachmentTimestampLower(int64_t timestamp) {
  transaction_set_attachment_timestamp_lower(iota_transaction, timestamp);
}

// Get the transaction attachment timestamp upper
int64_t TryteTransaction::attachmentTimestampUpper(void) {
  return transaction_attachment_timestamp_upper(iota_transaction);
}

// Set the transaction attachment timestamp upper
void TryteTransaction::setAttachmentTimestampUpper(int64_t timestamp) {
  transaction_set_attachment_timestamp_upper(iota_transaction, timestamp);
}

// Get the transaction nonce
std::string TryteTransaction::nonce(void) {
  auto trits = transaction_nonce(iota_transaction);
  std::string trytes(NUM_TRYTES_NONCE, '9');
  flex_trits_to_trytes((tryte_t *)trytes.data(), NUM_TRYTES_NONCE, trits,
                       NUM_TRITS_NONCE, NUM_TRITS_NONCE);
  return trytes;
}

// Set the transaction nonce (assign)
void TryteTransaction::setNonce(const std::string &trytes) {
  size_t flex_len = NUM_FLEX_TRITS_FOR_TRITS(NUM_TRITS_NONCE);
  flex_trit_t trits[flex_len];
  flex_trits_from_trytes(trits, NUM_TRITS_NONCE, (const tryte_t *)trytes.data(),
                         NUM_TRYTES_NONCE, NUM_TRYTES_NONCE);
  transaction_set_nonce(iota_transaction, trits);
}

// Get the transaction hash
std::string TryteTransaction::hash(void) {
  auto trits = transaction_hash(iota_transaction);
  std::string trytes(NUM_TRYTES_HASH, '9');
  flex_trits_to_trytes((tryte_t *)trytes.data(), NUM_TRYTES_HASH, trits,
                       NUM_TRITS_HASH, NUM_TRITS_HASH);
  return trytes;
}

// Set the transaction hash (assign)
void TryteTransaction::setHash(const std::string &trytes) {
  size_t flex_len = NUM_FLEX_TRITS_FOR_TRITS(NUM_TRITS_HASH);
  flex_trit_t trits[flex_len];
  flex_trits_from_trytes(trits, NUM_TRITS_HASH, (const tryte_t *)trytes.data(),
                         NUM_TRYTES_HASH, NUM_TRYTES_HASH);
  transaction_set_hash(iota_transaction, trits);
}

std::vector<flex_trit_t> TryteTransaction::serialize(void) {
  size_t num_bytes = NUM_FLEX_TRITS_FOR_TRITS(NUM_TRITS_SERIALIZED_TRANSACTION);
  std::vector<flex_trit_t> value(num_bytes);
  transaction_serialize_on_flex_trits(iota_transaction, value.data());
  return value;
}
}  // namespace model
}  // namespace iota
