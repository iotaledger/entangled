/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#include "common/model/cpptransaction.h"
#include "common/trinary/trit_array.h"

namespace model {
transaction::transaction() {
  iota_transaction = transaction_new();
}

transaction::transaction(std::string trytes) {
  iota_transaction = transaction_new();
  size_t to_len = trytes.size() * 3;
  size_t flex_len = flex_trits_num_for_trits(to_len);
  flex_trit_t trits[flex_len];
  tryte_to_flex_trit(trits, to_len, (const tryte_t *)trytes.data(), trytes.size(), trytes.size());
  transaction_deserialize_from_trits(iota_transaction, trits);
}

transaction::~transaction() {
  transaction_free(iota_transaction);
}

std::string transaction::get_signature(void) {
  auto trits = transaction_signature(iota_transaction);
  tryte_t trytes[2187];
  flex_trit_to_tryte(trytes, 2187, trits, 6561, 6561);
  return std::string(reinterpret_cast<char*>(trytes), 2187);
}

// Set the transaction signature (copy argument)
void transaction::set_signature(std::string trytes) {
  size_t flex_len = flex_trits_num_for_trits(6561);
  flex_trit_t trits[flex_len];
  tryte_to_flex_trit(trits, 6561, (const tryte_t *)trytes.data(), 2187, 2187);
  transaction_set_signature(iota_transaction, trits);
}

// Get the transaction message
std::string transaction::get_message(void) {
  auto trits = transaction_message(iota_transaction);
  tryte_t trytes[2187];
  flex_trit_to_tryte(trytes, 2187, trits, 6561, 6561);
  return std::string(reinterpret_cast<char*>(trytes), 2187);
}

// Set the transaction message (copy argument)
void transaction::set_message(std::string trytes) {
  size_t flex_len = flex_trits_num_for_trits(6561);
  flex_trit_t trits[flex_len];
  tryte_to_flex_trit(trits, 6561, (const tryte_t *)trytes.data(), 2187, 2187);
  transaction_set_message(iota_transaction, trits);
}

// Get the transaction address
std::string transaction::get_address(void) {
  auto trits = transaction_address(iota_transaction);
  tryte_t trytes[81];
  flex_trit_to_tryte(trytes, 81, trits, 243, 243);
  return std::string(reinterpret_cast<char*>(trytes), 81);
}

// Set the transaction address (copy argument)
void transaction::set_address(std::string trytes) {
  size_t flex_len = flex_trits_num_for_trits(243);
  flex_trit_t trits[flex_len];
  tryte_to_flex_trit(trits, 243, (const tryte_t *)trytes.data(), 81, 81);
  transaction_set_address(iota_transaction, trits);
}

// Get the transaction value
int64_t transaction::get_value(void) {
  return transaction_value(iota_transaction);
}

// Set the transaction value
void transaction::set_value(int64_t value) {
  transaction_set_value(iota_transaction, value);
}

// Get the transaction obsolete tag
std::string transaction::get_obsolete_tag(void) {
  auto trits = transaction_obsolete_tag(iota_transaction);
  tryte_t trytes[81];
  flex_trit_to_tryte(trytes, 81, trits, 243, 243);
  return std::string(reinterpret_cast<char*>(trytes), 81);
}

// Set the transaction obsolete tag
void transaction::set_obsolete_tag(std::string trytes) {
  size_t flex_len = flex_trits_num_for_trits(243);
  flex_trit_t trits[flex_len];
  tryte_to_flex_trit(trits, 243, (const tryte_t *)trytes.data(), 81, 81);
  transaction_set_obsolete_tag(iota_transaction, trits);
}

// Get the transaction timestamp
uint64_t transaction::get_timestamp(void) {
  return transaction_timestamp(iota_transaction);
}

// Set the transaction timestamp
void transaction::set_timestamp(uint64_t timestamp) {
  transaction_set_timestamp(iota_transaction, timestamp);
}

// Get the transaction current index
int64_t transaction::get_current_index(void) {
  return transaction_current_index(iota_transaction);
}

// Set the transaction current index
void transaction::set_current_index(int64_t index) {
  transaction_set_current_index(iota_transaction, index);
}

// Get the transaction last index
int64_t transaction::get_last_index(void) {
  return transaction_last_index(iota_transaction);
}

// Set the transaction last index
void transaction::set_last_index(int64_t last_index) {
  transaction_set_last_index(iota_transaction, last_index);
}

// Get the transaction bundle
std::string transaction::get_bundle(void) {
  auto trits = transaction_bundle(iota_transaction);
  tryte_t trytes[81];
  flex_trit_to_tryte(trytes, 81, trits, 243, 243);
  return std::string(reinterpret_cast<char*>(trytes), 81);
}

// Set the transaction bundle (copy argument)
void transaction::set_bundle(std::string trytes) {
  size_t flex_len = flex_trits_num_for_trits(243);
  flex_trit_t trits[flex_len];
  tryte_to_flex_trit(trits, 243, (const tryte_t *)trytes.data(), 81, 81);
  transaction_set_bundle(iota_transaction, trits);
}

// Get the transaction trunk
std::string transaction::get_trunk(void) {
  auto trits = transaction_trunk(iota_transaction);
  tryte_t trytes[81];
  flex_trit_to_tryte(trytes, 81, trits, 243, 243);
  return std::string(reinterpret_cast<char*>(trytes), 81);
}

// Set the transaction trunk (copy argument)
void transaction::set_trunk(std::string trytes) {
  size_t flex_len = flex_trits_num_for_trits(243);
  flex_trit_t trits[flex_len];
  tryte_to_flex_trit(trits, 243, (const tryte_t *)trytes.data(), 81, 81);
  transaction_set_trunk(iota_transaction, trits);
}

// Get the transaction branch
std::string transaction::get_branch(void) {
  auto trits = transaction_branch(iota_transaction);
  tryte_t trytes[81];
  flex_trit_to_tryte(trytes, 81, trits, 243, 243);
  return std::string(reinterpret_cast<char*>(trytes), 81);
}

// Set the transaction branch (copy argument)
void transaction::set_branch(std::string trytes) {
  size_t flex_len = flex_trits_num_for_trits(243);
  flex_trit_t trits[flex_len];
  tryte_to_flex_trit(trits, 243, (const tryte_t *)trytes.data(), 81, 81);
  transaction_set_branch(iota_transaction, trits);
}

// Get the transaction tag
std::string transaction::get_tag(void) {
  auto trits = transaction_branch(iota_transaction);
  tryte_t trytes[27];
  flex_trit_to_tryte(trytes, 27, trits, 81, 81);
  return std::string(reinterpret_cast<char*>(trytes), 27);
}

// Set the transaction tag (copy argument)
void transaction::set_tag(std::string trytes) {
  size_t flex_len = flex_trits_num_for_trits(81);
  flex_trit_t trits[flex_len];
  tryte_to_flex_trit(trits, 81, (const tryte_t *)trytes.data(), 27, 27);
  transaction_set_tag(iota_transaction, trits);
}

// Get the transaction attachement timestamp
int64_t transaction::get_attachment_timestamp(void) {
  return transaction_attachment_timestamp(iota_transaction);
}

// Set the transaction attachement timestamp
void transaction::set_attachment_timestamp(int64_t timestamp) {
  transaction_set_attachment_timestamp(iota_transaction, timestamp);
}

// Get the transaction attachement timestamp lower
int64_t transaction::get_attachment_timestamp_lower(void) {
  return transaction_attachment_timestamp_lower(iota_transaction);
}

// Set the transaction attachement timestamp lower
void transaction::set_attachment_timestamp_lower(int64_t timestamp) {
  transaction_set_attachment_timestamp_lower(iota_transaction, timestamp);
}

// Get the transaction attachement timestamp upper
int64_t transaction::get_attachment_timestamp_upper(void) {
  return transaction_attachment_timestamp_upper(iota_transaction);
}

// Set the transaction attachement timestamp upper
void transaction::set_attachment_timestamp_upper(int64_t timestamp) {
  transaction_set_attachment_timestamp_upper(iota_transaction, timestamp);
}

// Get the transaction nonce
std::string transaction::get_nonce(void) {
  auto trits = transaction_nonce(iota_transaction);
  tryte_t trytes[27];
  flex_trit_to_tryte(trytes, 27, trits, 81, 81);
  return std::string(reinterpret_cast<char*>(trytes), 27);
}

// Set the transaction nonce (copy argument)
void transaction::set_nonce(std::string trytes) {
  size_t flex_len = flex_trits_num_for_trits(81);
  flex_trit_t trits[flex_len];
  tryte_to_flex_trit(trits, 81, (const tryte_t *)trytes.data(), 27, 27);
  transaction_set_nonce(iota_transaction, trits);
}
}