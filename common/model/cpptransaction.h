/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#ifndef __COMMON_MODEL_CPPTRANSACTION_H_
#define __COMMON_MODEL_CPPTRANSACTION_H_

#include <string>
#include <vector>
#include "common/model/transaction.h"
#include "common/trinary/tryte.h"

namespace iota {
namespace model {

class Transaction {
 private:
  iota_transaction_t iota_transaction;

 public:
  Transaction();
  Transaction(const std::string& trytes);
  ~Transaction(void);

  std::string get_signature(void);
  // Set the transaction signature (copy argument)
  void set_signature(const std::string& signature);
  // Get the transaction message
  std::string get_message(void);
  // Set the transaction message (copy argument)
  void set_message(const std::string& message);
  // Get the transaction address
  std::string get_address(void);
  // Set the transaction address (copy argument)
  void set_address(const std::string& address);
  // Get the transaction value
  int64_t get_value(void);
  // Set the transaction value
  void set_value(int64_t value);
  // Get the transaction obsolete tag
  std::string get_obsolete_tag(void);
  // Set the transaction obsolete tag
  void set_obsolete_tag(const std::string& obsolete_tag);
  // Get the transaction timestamp
  uint64_t get_timestamp(void);
  // Set the transaction timestamp
  void set_timestamp(uint64_t timestamp);
  // Get the transaction current index
  int64_t get_current_index(void);
  // Set the transaction current index
  void set_current_index(int64_t index);
  // Get the transaction last index
  int64_t get_last_index(void);
  // Set the transaction last index
  void set_last_index(int64_t last_index);
  // Get the transaction bundle
  std::string get_bundle(void);
  // Set the transaction bundle (copy argument)
  void set_bundle(const std::string& bundle);
  // Get the transaction trunk
  std::string get_trunk(void);
  // Set the transaction trunk (copy argument)
  void set_trunk(const std::string& trunk);
  // Get the transaction branch
  std::string get_branch(void);
  // Set the transaction branch (copy argument)
  void set_branch(const std::string& branch);
  // Get the transaction tag
  std::string get_tag(void);
  // Set the transaction tag (copy argument)
  void set_tag(const std::string& tag);
  // Get the transaction attachement timestamp
  int64_t get_attachment_timestamp(void);
  // Set the transaction attachement timestamp
  void set_attachment_timestamp(int64_t timestamp);
  // Get the transaction attachement timestamp lower
  int64_t get_attachment_timestamp_lower(void);
  // Set the transaction attachement timestamp lower
  void set_attachment_timestamp_lower(int64_t timestamp);
  // Get the transaction attachement timestamp upper
  int64_t get_attachment_timestamp_upper(void);
  // Set the transaction attachement timestamp upper
  void set_attachment_timestamp_upper(int64_t timestamp);
  // Get the transaction nonce
  std::string get_nonce(void);
  // Set the transaction nonce (copy argument)
  void set_nonce(const std::string& nonce);

  std::vector<flex_trit_t> serialize(void);
};  // class transaction
}  // namespace model
}  // namespace iota

#endif  // __COMMON_MODEL_CPPTRANSACTION_H_