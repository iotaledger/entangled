#pragma once

#include <chrono>
#include <cstdint>
#include <memory>
#include <sstream>
#include <string>
#include <string_view>
#include <vector>

namespace iota {
namespace tanglescope {

namespace iri {

enum IRIMessageType {
  TX = 0,
  RSTAT,
  DNSCV,
  DNSCC,
  ANTN,
  MCTN,
  RNTN,
  LMHS,
  LMSI,
  LMI,
  SN,
  HMR,
  RTL,
  RTSL,
  RTSS,
  RTSV,
  RTSD,
  RTST,
};

/**!
 * Base class for all IRI ZMQ messages.
 */
class IRIMessage {
 public:
  virtual IRIMessageType type() = 0;

 protected:
  static std::vector<std::string> chunks(std::string_view s, char delim = ' ') {
    std::vector<std::string> elems;
    size_t pos = 0, ppos = 0;
    while ((pos = s.find(delim, ppos)) != std::string_view::npos) {
      elems.push_back(std::string{s.substr(ppos, pos - ppos)});
      ppos = pos + 1;
    }

    elems.push_back(std::string{s.substr(ppos)});

    return elems;
  }
};

/// Deserialises a string payload to the correct message implementation.
std::shared_ptr<iri::IRIMessage> payloadToMsg(std::string_view payload);

/**!
     Emitted when IRI marks a transaction as confirmed.
  */

class SNMessage : public IRIMessage {
 public:
  explicit SNMessage(std::string_view from) {
    auto chunks = IRIMessage::chunks(from);
    this->_milestoneIdx = std::stoull(chunks[0]);
    this->_hash = std::move(chunks[1]);
    this->_address = std::move(chunks[2]);
    this->_trunk = std::move(chunks[3]);
    this->_branch = std::move(chunks[4]);
    this->_bundle = std::move(chunks[5]);
  }

  inline IRIMessageType type() { return IRIMessageType::SN; }

 public:
  const std::string& hash() { return this->_hash; }
  uint64_t milestoneIdx() { return this->_milestoneIdx; }
  const std::string& address() { return this->_address; }
  const std::string& bundle() { return this->_bundle; }
  const std::string& trunk() { return this->_trunk; }
  const std::string& branch() { return this->_branch; }

 private:
  std::string _hash;
  uint64_t _milestoneIdx;
  std::string _address;
  std::string _trunk;
  std::string _branch;
  std::string _bundle;
};

/**!
 * Emitted when IRI has received a new Transaction.
 */
class TXMessage : public IRIMessage {
 public:
  explicit TXMessage(std::string_view from) {
    auto chunks = IRIMessage::chunks(from);

    this->_hash = std::move(chunks[0]);
    this->_address = std::move(chunks[1]);
    this->_value = std::stoll(chunks[2]);
    this->_obsoleteTag = std::move(chunks[3]);

    {
      std::chrono::milliseconds ts(std::stoull(chunks[4]) * 1000);
      _timestamp = std::chrono::system_clock::time_point(ts);
    }

    this->_currentIndex = std::stoull(chunks[5]);
    this->_lastIndex = std::stoull(chunks[6]);
    this->_bundle = std::move(chunks[7]);
    this->_trunk = std::move(chunks[8]);
    this->_branch = std::move(chunks[9]);

    {
      std::chrono::milliseconds ts(std::stoull(chunks[10]));
      _arrivalTime = std::chrono::system_clock::time_point(ts);
    }
  }
  inline IRIMessageType type() { return IRIMessageType::TX; }

 public:
  const std::string& hash() { return this->_hash; }
  const std::string& address() { return this->_address; }
  int64_t value() { return this->_value; }
  const std::string& obsoleteTag() { return this->_obsoleteTag; }
  const std::chrono::system_clock::time_point& timestamp() { return this->_timestamp; }
  uint64_t currentIndex() { return this->_currentIndex; }
  uint64_t lastIndex() { return this->_lastIndex; }
  const std::string& bundle() { return this->_bundle; }
  const std::string& trunk() { return this->_trunk; }
  const std::string& branch() { return this->_branch; }
  const std::chrono::system_clock::time_point& arrivalTime() { return this->_arrivalTime; }

 private:
  std::string _hash;
  std::string _address;
  int64_t _value;
  std::string _obsoleteTag;
  std::chrono::system_clock::time_point _timestamp;
  uint64_t _currentIndex;
  uint64_t _lastIndex;
  std::string _bundle;
  std::string _trunk;
  std::string _branch;
  std::chrono::system_clock::time_point _arrivalTime;
};

class LMHSMessage : public IRIMessage {
 public:
  explicit LMHSMessage(std::string_view from) {
    auto chunks = IRIMessage::chunks(from);
    this->_latestSolidMilestoneHash = std::move(chunks[0]);
  }

  inline IRIMessageType type() { return IRIMessageType::LMHS; }

 public:
  const std::string& latestSolidMilestoneHash() { return this->_latestSolidMilestoneHash; }

 private:
  std::string _latestSolidMilestoneHash;
};

class LMSIMessage : public IRIMessage {
 public:
  explicit LMSIMessage(std::string_view from) {
    auto chunks = IRIMessage::chunks(from);
    this->_latestSolidMilestoneIndex = std::stoull(std::move(chunks[1]));
  }

  inline IRIMessageType type() { return IRIMessageType::LMSI; }

 public:
  uint64_t latestSolidMilestoneIndex() { return this->_latestSolidMilestoneIndex; }

 private:
  uint64_t _latestSolidMilestoneIndex;
};

class RSTATMessage : public IRIMessage {
 public:
  explicit RSTATMessage(std::string_view from) {
    auto chunks = IRIMessage::chunks(from);
    this->_toProcess = std::stoull(chunks[0]);
    this->_toBroadcast = std::stoull(chunks[1]);
    this->_toRequest = std::stoull(chunks[2]);
    this->_toReply = std::stoull(chunks[3]);
    this->_totalTransactions = std::stoull(chunks[4]);
  }

  inline IRIMessageType type() { return IRIMessageType::RSTAT; }

 public:
  uint64_t toProcess() { return this->_toProcess; }
  uint64_t toBroadcast() { return this->_toBroadcast; }
  uint64_t toRequest() { return this->_toRequest; }
  uint64_t toReply() { return this->_toReply; }
  uint64_t totalTransactions() { return this->_totalTransactions; }

 private:
  uint32_t _toProcess;
  uint32_t _toBroadcast;
  uint32_t _toRequest;
  uint32_t _toReply;
  uint32_t _totalTransactions;
};
}  // namespace iri
}  // namespace tanglescope
}  // namespace iota
