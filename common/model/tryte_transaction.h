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

class TryteTransaction {
 private:
  iota_transaction_t iota_transaction;

 public:
  TryteTransaction();
  TryteTransaction(const std::string& trytes);
  ~TryteTransaction(void);

  std::string signature(void);
  // Set the transaction signature (assign)
  void setSignature(const std::string& signature);
  // Get the transaction message
  std::string message(void);
  // Set the transaction message (assign)
  void setMessage(const std::string& message);
  // Get the transaction address
  std::string address(void);
  // Set the transaction address (assign)
  void setAddress(const std::string& address);
  // Get the transaction value
  int64_t value(void);
  // Set the transaction value
  void setValue(int64_t value);
  // Get the transaction obsolete tag
  std::string obsoleteTag(void);
  // Set the transaction obsolete tag
  void setObsoleteTag(const std::string& obsolete_tag);
  // Get the transaction timestamp
  uint64_t timestamp(void);
  // Set the transaction timestamp
  void setTimestamp(uint64_t timestamp);
  // Get the transaction current index
  int64_t currentIndex(void);
  // Set the transaction current index
  void setCurrentIndex(int64_t index);
  // Get the transaction last index
  int64_t lastIndex(void);
  // Set the transaction last index
  void setLastIndex(int64_t last_index);
  // Get the transaction bundle
  std::string bundle(void);
  // Set the transaction bundle (assign)
  void setBundle(const std::string& bundle);
  // Get the transaction trunk
  std::string trunk(void);
  // Set the transaction trunk (assign)
  void setTrunk(const std::string& trunk);
  // Get the transaction branch
  std::string branch(void);
  // Set the transaction branch (assign)
  void setBranch(const std::string& branch);
  // Get the transaction tag
  std::string tag(void);
  // Set the transaction tag (assign)
  void setTag(const std::string& tag);
  // Get the transaction attachment timestamp
  int64_t attachmentTimestamp(void);
  // Set the transaction attachment timestamp
  void setAttachmentTimestamp(int64_t timestamp);
  // Get the transaction attachment timestamp lower
  int64_t attachmentTimestampLower(void);
  // Set the transaction attachment timestamp lower
  void setAttachmentTimestampLower(int64_t timestamp);
  // Get the transaction attachment timestamp upper
  int64_t attachmentTimestampUpper(void);
  // Set the transaction attachment timestamp upper
  void setAttachmentTimestampUpper(int64_t timestamp);
  // Get the transaction nonce
  std::string nonce(void);
  // Set the transaction nonce (assign)
  void setNonce(const std::string& nonce);
  // Get the transaction hash
  std::string hash(void);
  // Set the transaction hash (assign)
  void setHash(const std::string& hash);

  std::vector<flex_trit_t> serialize(void);
};  // class transaction
}  // namespace model
}  // namespace iota

#endif  // __COMMON_MODEL_CPPTRANSACTION_H_
