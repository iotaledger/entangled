/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#include "cppclient/api_json.h"

#include <algorithm>
#include <cstdint>
#include <string>
#include <tuple>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <vector>

#include <gflags/gflags.h>
#include <glog/logging.h>
#include <boost/move/move.hpp>
#include <boost/range/adaptors.hpp>
#include <boost/range/algorithm/copy.hpp>
#include <boost/range/combine.hpp>
#include <nlohmann/json.hpp>
#include "common/helpers/digest.h"
#include "common/model/transaction.h"
#include "common/model/tryte_transaction.h"

using json = nlohmann::json;
using boost::adaptors::filtered;
using boost::adaptors::transformed;
using boost::adaptors::uniqued;
using iota::model::TryteTransaction;

DEFINE_uint32(maxNumAddressesForGetBalances, 1000, "Maximum number of addresses to query for in 'getBalances'");

DEFINE_uint32(maxQuerySizeFindTransactions, 500,
              "Maximum number of addresses/bundles/approvees to query for in "
              "'findTransactions'");

DEFINE_uint32(maxQuerySizeGetTrytes, 500,
              "Maximum number of hashes to query for in "
              "'getTrytes'");

DEFINE_uint32(maxQuerySizeGetInclusionState, 1000,
              "Maximum number of tails to query for in "
              "'getInclusionStates'");

namespace cppclient {

IotaJsonAPI::IotaJsonAPI(bool useSsl) : _useSsl(useSsl) {}

bool IotaJsonAPI::isSsl() const { return _useSsl; }

void IotaJsonAPI::setSslPemPath(const std::string& sslPemPath) { _sslPemPath = sslPemPath; }

const std::string& IotaJsonAPI::sslPemPath() const { return _sslPemPath; }

bool IotaJsonAPI::isNodeSolid() {
  VLOG(3) << __FUNCTION__;
  auto maybeNodeInfo = getNodeInfo();
  if (!maybeNodeInfo.has_value()) {
    LOG(INFO) << __FUNCTION__ << " request failed.";
    return false;
  }
  auto ni = maybeNodeInfo.value();
  if (ni.latestMilestoneIndex != ni.latestSolidMilestoneIndex) {
    return false;
  }

  auto tx = getTransactions({ni.latestMilestone}, false)[0];
  auto now = std::chrono::system_clock::now();

  if ((now - tx.timestamp) > std::chrono::seconds(300)) {
    return false;
  }

  return true;
}

nonstd::optional<NodeInfo> IotaJsonAPI::getNodeInfo() {
  VLOG(3) << __FUNCTION__;
  json req;
  req["command"] = "getNodeInfo";

  auto maybeResponse = post(std::move(req));
  if (!maybeResponse) {
    LOG(INFO) << __FUNCTION__ << " request failed.";
    return {};
  }
  auto response = maybeResponse.value();

  if (response["latestMilestone"].is_null()) {
    LOG(INFO) << __FUNCTION__ << " request failed.";
    return {};
  }

  return {
      {response["latestMilestone"], response["latestMilestoneIndex"], response["latestSolidSubtangleMilestoneIndex"]}};
}

nonstd::optional<std::unordered_map<std::string, uint64_t>> IotaJsonAPI::getBalances(
    const std::vector<std::string>& addresses) {
  VLOG(3) << __FUNCTION__;
  std::unordered_map<std::string, uint64_t> result;
  json req;
  req["command"] = "getBalances";
  req["threshold"] = 100;

  uint32_t currAddressCount = 0;
  while (currAddressCount < addresses.size()) {
    auto numAddressesToQuery = (addresses.size() - currAddressCount) > FLAGS_maxNumAddressesForGetBalances
                                   ? FLAGS_maxNumAddressesForGetBalances
                                   : (addresses.size() - currAddressCount);
    auto start = addresses.begin() + currAddressCount;
    auto currAddresses = std::vector<std::string>(start, start + numAddressesToQuery);
    req["addresses"] = currAddresses;

    auto maybeResponse = post(req);

    if (!maybeResponse) {
      LOG(INFO) << __FUNCTION__ << " request failed.";
      return {};
    }

    auto& response = maybeResponse.value();

    if (response["balances"].is_null()) {
      return {};
    }
    auto balances = response["balances"].get<std::vector<std::string>>();

    for (const auto& tup : boost::combine(currAddresses, balances)) {
      std::string add, bal;
      boost::tie(add, bal) = tup;
      result.emplace(std::move(add), std::stoull(bal));
    }

    currAddressCount += numAddressesToQuery;
  }

  return {result};
}

std::vector<std::string> IotaJsonAPI::findTransactions(nonstd::optional<std::vector<std::string>> addresses,
                                                       nonstd::optional<std::vector<std::string>> bundles,
                                                       nonstd::optional<std::vector<std::string>> approvees) {
  VLOG(3) << __FUNCTION__;
  if (!addresses.has_value() && !bundles.has_value() && !approvees.has_value()) {
    return {};
  }

  json req;
  req["command"] = "findTransactions";

  std::vector<std::string> txs;

  uint32_t currAddressCount = 0;
  uint32_t currBundleCount = 0;
  uint32_t currApproveeCount = 0;
  uint32_t maxAddresses = addresses.has_value() ? addresses.value().size() : 0;
  uint32_t maxBundles = bundles.has_value() ? bundles.value().size() : 0;
  uint32_t maxApprovees = approvees.has_value() ? approvees.value().size() : 0;

  while (currAddressCount < maxAddresses || currBundleCount < maxBundles || currApproveeCount < maxApprovees) {
    if (currAddressCount < maxAddresses) {
      auto currAddresses = nextBatch(addresses.value(), currAddressCount, FLAGS_maxQuerySizeFindTransactions);
      req["addresses"] = std::move(currAddresses);
    }

    if (currBundleCount < maxBundles) {
      auto currBundles = nextBatch(bundles.value(), currBundleCount, FLAGS_maxQuerySizeFindTransactions);
      req["bundles"] = std::move(currBundles);
    }

    if (currApproveeCount < maxApprovees) {
      auto currApprovees = nextBatch(bundles.value(), currApproveeCount, FLAGS_maxQuerySizeFindTransactions);
      req["approvees"] = std::move(currApprovees);
    }

    auto maybeResponse = post(std::move(req));
    if (!maybeResponse) {
      LOG(INFO) << __FUNCTION__ << " request failed.";
      return {};
    }

    if (!maybeResponse.value()["hashes"].is_null()) {
      auto resVec = maybeResponse.value()["hashes"].get<std::vector<std::string>>();
      std::copy(std::begin(resVec), std::end(resVec), std::back_inserter(txs));
    } else {
      LOG(INFO) << __FUNCTION__ << " request failed.";
      return {};
    }
  }

  return txs;
}
std::vector<std::string> IotaJsonAPI::getTrytes(const std::vector<std::string>& hashes) {
  VLOG(3) << __FUNCTION__;
  json req;
  req["command"] = "getTrytes";

  uint32_t currHashesCount = 0;
  std::vector<std::string> trytes;

  while (currHashesCount < hashes.size()) {
    auto currHashes = nextBatch(hashes, currHashesCount, FLAGS_maxQuerySizeFindTransactions);
    req["hashes"] = currHashes;

    auto maybeResponse = post(std::move(req));
    if (!maybeResponse) {
      LOG(INFO) << __FUNCTION__ << " request failed.";
      return {};
    }

    if (!maybeResponse.value()["trytes"].is_null()) {
      auto res = maybeResponse.value()["trytes"].get<std::vector<std::string>>();
      trytes.insert(std::end(trytes), std::begin(res), std::end(res));
    } else {
      LOG(INFO) << __FUNCTION__ << " request failed.";
      return {};
    }
  }
  return trytes;
}

std::vector<Transaction> IotaJsonAPI::getTransactions(const std::vector<std::string>& hashes, bool includeMessages) {
  VLOG(3) << __FUNCTION__;
  auto trytes = getTrytes(hashes);

  if (trytes.empty()) {
    return {};
  }

  std::vector<Transaction> txs;
  std::chrono::system_clock::time_point epoch;

  boost::copy(trytes | transformed([&epoch, includeMessages](const std::string& trytes) -> Transaction {
                TryteTransaction* tx = new TryteTransaction(trytes);

                // We could also rely on the ordering of the hashes argument here.
                auto hash = iota_digest(trytes.c_str());
                std::string sHash = std::string(reinterpret_cast<char*>(hash), 81);
                std::free(hash);

                auto sAddress = tx->address();
                auto sBundle = tx->bundle();
                auto sTrunk = tx->trunk();
                auto sOptMessage = includeMessages ? nonstd::optional<std::string>(tx->message()) : nonstd::nullopt;

                std::chrono::seconds sinceEpoch(tx->timestamp());

                return {sHash,           sAddress, tx->value(), epoch + sinceEpoch, tx->currentIndex(),
                        tx->lastIndex(), sBundle,  sTrunk,      sOptMessage};
              }),
              boost::back_move_inserter(txs));

  // transaction_free(tx);

  return txs;
}

std::unordered_multimap<std::string, Bundle> IotaJsonAPI::getConfirmedBundlesForAddresses(
    const std::vector<std::string>& addresses, bool includeMessages) {
  VLOG(3) << __FUNCTION__;
  // 1. Get all transactions for address [findTransactions, getTransactions]
  auto txHashes = findTransactions(addresses, {}, {});
  auto transactions = getTransactions(txHashes, false);

  // 2. Filter unique bundles from these []
  std::vector<std::string> bundles;
  boost::copy(transactions | transformed([](const Transaction& tx) { return tx.bundleHash; }) | uniqued,
              boost::back_move_inserter(bundles));

  // 3. Materialise all bundles [findTransactions, getTransactions]
  txHashes = findTransactions({}, bundles, {});
  transactions = getTransactions(txHashes, includeMessages);

  // 4. Filter unconfirmed bundles [getNodeInfo, getInclusionStates]
  std::vector<std::string> tails;
  boost::copy(transactions | filtered([](const Transaction& tx) { return tx.currentIndex == 0; }) |
                  transformed([](const Transaction& tx) { return tx.hash; }),
              boost::back_move_inserter(tails));

  auto confirmedTails = filterConfirmedTails(tails, {});

  std::vector<Bundle> confirmedBundles;
  std::unordered_multimap<std::string, Bundle> confirmedBundlesMap;
  std::unordered_map<std::string, Transaction> transactionsByHash;

  for (auto& tx : transactions) {
    transactionsByHash.emplace(tx.hash, std::move(tx));
  }

  for (const std::string& tail : confirmedTails) {
    Bundle bundle;
    auto tx = std::move(transactionsByHash[std::move(tail)]);

    while (tx.currentIndex != tx.lastIndex) {
      auto& trunk = tx.trunk;
      auto& next = transactionsByHash[trunk];

      bundle.push_back(std::move(tx));
      tx = std::move(next);
    }
    bundle.push_back(std::move(tx));
    confirmedBundles.push_back(std::move(bundle));
  }

  for (const auto& address : addresses) {
    for (const auto& b : confirmedBundles) {
      if (std::find_if(b.begin(), b.end(),
                       [address](const Transaction& tx) -> bool { return address == tx.address; }) != b.end()) {
        confirmedBundlesMap.emplace(std::pair<std::string, Bundle>(address, b));
      }
    }
  }

  return confirmedBundlesMap;
}

std::unordered_set<std::string> IotaJsonAPI::filterConfirmedTails(const std::vector<std::string>& tails,
                                                                  const nonstd::optional<std::string>& reference) {
  VLOG(3) << __FUNCTION__;
  json req;
  req["command"] = "getInclusionStates";

  if (reference.has_value()) {
    req["tips"] = std::vector<std::string>{reference.value()};
  } else {
    auto niOptional = getNodeInfo();
    if (!niOptional.has_value()) {
      LOG(INFO) << __FUNCTION__ << " request failed.";
      return {};
    }
    auto ni = niOptional.value();
    req["tips"] = std::vector<std::string>{ni.latestMilestone};
  }

  uint32_t currTailCount = 0;

  std::vector<bool> states;

  while (currTailCount < tails.size()) {
    auto currTails = nextBatch(tails, currTailCount, FLAGS_maxQuerySizeGetInclusionState);

    req["transactions"] = currTails;

    auto maybeResponse = post(std::move(req));
    if (!maybeResponse) {
      LOG(INFO) << __FUNCTION__ << " request failed.";
      return {};
    }

    auto& response = maybeResponse.value();

    if (response["states"].is_null()) {
      LOG(INFO) << __FUNCTION__ << " request failed.";
      return {};
    } else {
      auto currVec = response["states"].get<std::vector<bool>>();
      std::copy(std::begin(currVec), std::end(currVec), std::back_inserter(states));
    }
  }

  std::unordered_set<std::string> confirmedTails;

  for (auto const& tpl : boost::combine(tails, states)) {
    if (tpl.get<1>()) {
      confirmedTails.emplace(tpl.get<0>());
    }
  }

  return confirmedTails;
}

nonstd::optional<GetTransactionsToApproveResponse> IotaJsonAPI::getTransactionsToApprove(
    uint32_t depth, const nonstd::optional<std::string>& reference) {
  VLOG(3) << __FUNCTION__;
  json req;

  req["command"] = "getTransactionsToApprove";
  req["depth"] = depth;

  if (reference.has_value()) {
    req["reference"] = reference.value();
  }

  auto maybeResponse = post(std::move(req));

  if (!maybeResponse) {
    LOG(INFO) << __FUNCTION__ << " request failed.";
    return {};
  }

  auto& response = maybeResponse.value();

  return {{response["trunkTransaction"], response["branchTransaction"], response["duration"]}};
}

std::vector<std::string> IotaJsonAPI::attachToTangle(const std::string& trunkTransaction,
                                                     const std::string& branchTransaction, size_t minWeightMagnitude,
                                                     const std::vector<std::string>& trytes) {
  VLOG(3) << __FUNCTION__;
  json req;
  req["command"] = "attachToTangle";
  req["trunkTransaction"] = trunkTransaction;
  req["branchTransaction"] = branchTransaction;
  req["minWeightMagnitude"] = minWeightMagnitude;

  // FIXME(th0br0) should decode trytes and not trust input ordering
  std::vector<std::string> localTrytes = trytes;
  std::reverse(std::begin(localTrytes), std::end(localTrytes));
  req["trytes"] = localTrytes;

  auto maybeResponse = post(std::move(req));

  if (!maybeResponse) {
    LOG(INFO) << __FUNCTION__ << " request failed.";
    return {};
  }

  if (maybeResponse.value()["trytes"].is_null()) {
    return {};
  }

  return maybeResponse.value()["trytes"].get<std::vector<std::string>>();
}

bool IotaJsonAPI::storeTransactions(const std::vector<std::string>& trytes) {
  VLOG(3) << __FUNCTION__;
  json req;
  req["command"] = "storeTransactions";
  req["trytes"] = trytes;

  auto maybeResponse = post(std::move(req));
  return maybeResponse && maybeResponse.has_value();
}

bool IotaJsonAPI::broadcastTransactions(const std::vector<std::string>& trytes) {
  VLOG(3) << __FUNCTION__;
  json req;
  req["command"] = "broadcastTransactions";
  req["trytes"] = trytes;

  auto maybeResponse = post(std::move(req));
  return maybeResponse && maybeResponse.has_value();
}

std::unordered_set<std::string> IotaJsonAPI::filterConsistentTails(const std::vector<std::string>& tails) {
  std::unordered_set<std::string> ret;
  VLOG(3) << __FUNCTION__;
  for (const auto& tail : tails) {
    json req;
    req["command"] = "checkConsistency";
    req["tails"] = std::vector<std::string>({tail});

    auto maybeResponse = post(std::move(req));

    if (!maybeResponse.has_value()) {
      continue;
    }

    if ((!maybeResponse.value()["state"].is_null()) && maybeResponse.value()["state"].get<bool>()) {
      ret.insert(tail);
    }
  }

  return ret;
}

nonstd::optional<GetInclusionStatesResponse> IotaJsonAPI::getInclusionStates(const std::vector<std::string>& trans,
                                                                             const std::vector<std::string>& tips) {
  VLOG(3) << __FUNCTION__;
  json req;
  req["command"] = "getInclusionStates";
  req["transactions"] = trans;
  req["tips"] = tips;

  auto maybeResponse = post(std::move(req));

  if (!maybeResponse) {
    LOG(INFO) << __FUNCTION__ << " request failed.";
    return {};
  }

  auto& response = maybeResponse.value();
  if (response["states"].is_null()) {
    return {};
  }

  return nonstd::optional<GetInclusionStatesResponse>({response["states"].get<std::vector<bool>>()});
}

nonstd::optional<WereAddressesSpentFromResponse> IotaJsonAPI::wereAddressesSpentFrom(
    const std::vector<std::string>& addresses) {
  VLOG(3) << __FUNCTION__;
  json req;
  req["command"] = "wereAddressesSpentFrom";
  req["addresses"] = addresses;

  auto maybeResponse = post(std::move(req));

  if (!maybeResponse) {
    LOG(INFO) << __FUNCTION__ << " request failed.";
    return {};
  }

  auto& response = maybeResponse.value();
  if (response["states"].is_null()) {
    return {};
  }

  return nonstd::optional<WereAddressesSpentFromResponse>({response["states"].get<std::vector<bool>>()});
}

template <typename T>
std::vector<T> IotaJsonAPI::nextBatch(const std::vector<T>& vec, uint32_t& numBatchedEntries, uint32_t batchSize) {
  auto numToQuery = (vec.size() - numBatchedEntries) > batchSize ? batchSize : (vec.size() - numBatchedEntries);
  auto startEntry = vec.begin() + numBatchedEntries;
  auto currVec = std::vector<T>(startEntry, startEntry + numToQuery);

  numBatchedEntries += currVec.size();

  return currVec;
}

}  // namespace cppclient
