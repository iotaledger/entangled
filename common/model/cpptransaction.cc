/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#include "common/model/cpptransaction.h"
#include "common/trinary/trit_array.h"

namespace iota {
namespace model {
Transaction::Transaction() { iota_transaction = transaction_new(); }

Transaction::Transaction(const std::string &trytes) {
  iota_transaction = transaction_new();
  size_t to_len = trytes.size() * 3;
  size_t flex_len = flex_trits_num_for_trits(to_len);
  flex_trit_t trits[flex_len];
  tryte_to_flex_trit(trits, to_len, (const tryte_t *)trytes.data(),
                     trytes.size(), trytes.size());
  transaction_deserialize_from_trits(iota_transaction, trits);
}

Transaction::~Transaction() { transaction_free(iota_transaction); }

std::string Transaction::get_signature(void) {
  auto trits = transaction_signature(iota_transaction);
  tryte_t trytes[2187];
  flex_trit_to_tryte(trytes, 2187, trits, 6561, 6561);
  return std::string(reinterpret_cast<char *>(trytes), 2187);
}

// Set the transaction signature (copy argument)
void Transaction::set_signature(const std::string &trytes) {
  size_t flex_len = flex_trits_num_for_trits(6561);
  flex_trit_t trits[flex_len];
  tryte_to_flex_trit(trits, 6561, (const tryte_t *)trytes.data(), 2187, 2187);
  transaction_set_signature(iota_transaction, trits);
}

// Get the transaction message
std::string Transaction::get_message(void) {
  auto trits = transaction_message(iota_transaction);
  tryte_t trytes[2187];
  flex_trit_to_tryte(trytes, 2187, trits, 6561, 6561);
  return std::string(reinterpret_cast<char *>(trytes), 2187);
}

// Set the transaction message (copy argument)
void Transaction::set_message(const std::string &trytes) {
  size_t flex_len = flex_trits_num_for_trits(6561);
  flex_trit_t trits[flex_len];
  tryte_to_flex_trit(trits, 6561, (const tryte_t *)trytes.data(), 2187, 2187);
  transaction_set_message(iota_transaction, trits);
}

// Get the transaction address
std::string Transaction::get_address(void) {
  auto trits = transaction_address(iota_transaction);
  tryte_t trytes[81];
  flex_trit_to_tryte(trytes, 81, trits, 243, 243);
  return std::string(reinterpret_cast<char *>(trytes), 81);
}

// Set the transaction address (copy argument)
void Transaction::set_address(const std::string &trytes) {
  size_t flex_len = flex_trits_num_for_trits(243);
  flex_trit_t trits[flex_len];
  tryte_to_flex_trit(trits, 243, (const tryte_t *)trytes.data(), 81, 81);
  transaction_set_address(iota_transaction, trits);
}

// Get the transaction value
int64_t Transaction::get_value(void) {
  return transaction_value(iota_transaction);
}

// Set the transaction value
void Transaction::set_value(int64_t value) {
  transaction_set_value(iota_transaction, value);
}

// Get the transaction obsolete tag
std::string Transaction::get_obsolete_tag(void) {
  auto trits = transaction_obsolete_tag(iota_transaction);
  tryte_t trytes[81];
  flex_trit_to_tryte(trytes, 81, trits, 243, 243);
  return std::string(reinterpret_cast<char *>(trytes), 81);
}

// Set the transaction obsolete tag
void Transaction::set_obsolete_tag(const std::string &trytes) {
  size_t flex_len = flex_trits_num_for_trits(243);
  flex_trit_t trits[flex_len];
  tryte_to_flex_trit(trits, 243, (const tryte_t *)trytes.data(), 81, 81);
  transaction_set_obsolete_tag(iota_transaction, trits);
}

// Get the transaction timestamp
uint64_t Transaction::get_timestamp(void) {
  return transaction_timestamp(iota_transaction);
}

// Set the transaction timestamp
void Transaction::set_timestamp(uint64_t timestamp) {
  transaction_set_timestamp(iota_transaction, timestamp);
}

// Get the transaction current index
int64_t Transaction::get_current_index(void) {
  return transaction_current_index(iota_transaction);
}

// Set the transaction current index
void Transaction::set_current_index(int64_t index) {
  transaction_set_current_index(iota_transaction, index);
}

// Get the transaction last index
int64_t Transaction::get_last_index(void) {
  return transaction_last_index(iota_transaction);
}

// Set the transaction last index
void Transaction::set_last_index(int64_t last_index) {
  transaction_set_last_index(iota_transaction, last_index);
}

// Get the transaction bundle
std::string Transaction::get_bundle(void) {
  auto trits = transaction_bundle(iota_transaction);
  tryte_t trytes[81];
  flex_trit_to_tryte(trytes, 81, trits, 243, 243);
  return std::string(reinterpret_cast<char *>(trytes), 81);
}

// Set the transaction bundle (copy argument)
void Transaction::set_bundle(const std::string &trytes) {
  size_t flex_len = flex_trits_num_for_trits(243);
  flex_trit_t trits[flex_len];
  tryte_to_flex_trit(trits, 243, (const tryte_t *)trytes.data(), 81, 81);
  transaction_set_bundle(iota_transaction, trits);
}

// Get the transaction trunk
std::string Transaction::get_trunk(void) {
  auto trits = transaction_trunk(iota_transaction);
  tryte_t trytes[81];
  flex_trit_to_tryte(trytes, 81, trits, 243, 243);
  return std::string(reinterpret_cast<char *>(trytes), 81);
}

// Set the transaction trunk (copy argument)
void Transaction::set_trunk(const std::string &trytes) {
  size_t flex_len = flex_trits_num_for_trits(243);
  flex_trit_t trits[flex_len];
  tryte_to_flex_trit(trits, 243, (const tryte_t *)trytes.data(), 81, 81);
  transaction_set_trunk(iota_transaction, trits);
}

// Get the transaction branch
std::string Transaction::get_branch(void) {
  auto trits = transaction_branch(iota_transaction);
  tryte_t trytes[81];
  flex_trit_to_tryte(trytes, 81, trits, 243, 243);
  return std::string(reinterpret_cast<char *>(trytes), 81);
}

// Set the transaction branch (copy argument)
void Transaction::set_branch(const std::string &trytes) {
  size_t flex_len = flex_trits_num_for_trits(243);
  flex_trit_t trits[flex_len];
  tryte_to_flex_trit(trits, 243, (const tryte_t *)trytes.data(), 81, 81);
  transaction_set_branch(iota_transaction, trits);
}

// Get the transaction tag
std::string Transaction::get_tag(void) {
  auto trits = transaction_branch(iota_transaction);
  tryte_t trytes[27];
  flex_trit_to_tryte(trytes, 27, trits, 81, 81);
  return std::string(reinterpret_cast<char *>(trytes), 27);
}

// Set the transaction tag (copy argument)
void Transaction::set_tag(const std::string &trytes) {
  size_t flex_len = flex_trits_num_for_trits(81);
  flex_trit_t trits[flex_len];
  tryte_to_flex_trit(trits, 81, (const tryte_t *)trytes.data(), 27, 27);
  transaction_set_tag(iota_transaction, trits);
}

// Get the transaction attachement timestamp
int64_t Transaction::get_attachment_timestamp(void) {
  return transaction_attachment_timestamp(iota_transaction);
}

// Set the transaction attachement timestamp
void Transaction::set_attachment_timestamp(int64_t timestamp) {
  transaction_set_attachment_timestamp(iota_transaction, timestamp);
}

// Get the transaction attachement timestamp lower
int64_t Transaction::get_attachment_timestamp_lower(void) {
  return transaction_attachment_timestamp_lower(iota_transaction);
}

// Set the transaction attachement timestamp lower
void Transaction::set_attachment_timestamp_lower(int64_t timestamp) {
  transaction_set_attachment_timestamp_lower(iota_transaction, timestamp);
}

// Get the transaction attachement timestamp upper
int64_t Transaction::get_attachment_timestamp_upper(void) {
  return transaction_attachment_timestamp_upper(iota_transaction);
}

// Set the transaction attachement timestamp upper
void Transaction::set_attachment_timestamp_upper(int64_t timestamp) {
  transaction_set_attachment_timestamp_upper(iota_transaction, timestamp);
}

// Get the transaction nonce
std::string Transaction::get_nonce(void) {
  auto trits = transaction_nonce(iota_transaction);
  tryte_t trytes[27];
  flex_trit_to_tryte(trytes, 27, trits, 81, 81);
  return std::string(reinterpret_cast<char *>(trytes), 27);
}

// Set the transaction nonce (copy argument)
void Transaction::set_nonce(const std::string &trytes) {
  size_t flex_len = flex_trits_num_for_trits(81);
  flex_trit_t trits[flex_len];
  tryte_to_flex_trit(trits, 81, (const tryte_t *)trytes.data(), 27, 27);
  transaction_set_nonce(iota_transaction, trits);
}

std::vector<flex_trit_t> Transaction::serialize(void) {
  size_t num_bytes = flex_trits_num_for_trits(NUM_TRITS_SERIALIZED_TRANSACTION);
  std::vector<flex_trit_t> value(num_bytes);
  transaction_serialize_on_flex_trits(iota_transaction, value.data());
  return value;
}
}  // namespace model
}  // namespace iota
