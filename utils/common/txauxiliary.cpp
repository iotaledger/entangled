
#include "txauxiliary.hpp"
#include <set>
#include <glog/logging.h>
#include <iota/utils/common/tangledb.hpp>

namespace iota {
namespace utils {
namespace txAuxiliary {

pplx::task<void> removeConfirmedTransactions(
    std::weak_ptr<api::IRIClient> client, const std::vector<std::string>& tips,
    std::vector<std::string>& txs) {
  if (auto clientSharedPtr = client.lock()) {
    return clientSharedPtr->getInclusionStates(txs, tips).then(
        [&](api::GetInclusionStatesResponse response) {
          if (response.states.empty()) {
            txs.clear();
            return;
          }

          auto states = std::move(response.states);
          auto idx = 0;
          txs.erase(std::remove_if(txs.begin(), txs.end(),
                                   [&idx, &states](std::string hash) {
                                     return states[idx++];
                                   }),
                    txs.end());
        });
  }
}

pplx::task<std::set<std::string>> getUnconfirmedTXs(
    std::weak_ptr<api::IRIClient> client, std::shared_ptr<iri::TXMessage> tx) {
  using namespace std;

  if (auto clientSharedPtr = client.lock()) {
    return clientSharedPtr->getNodeInfo().then([=](api::GetNodeInfoResponse
                                                       nodeInfo) {
      auto tips = {nodeInfo.latestMilestone};
      set<string> res;
      vector<string> currentLevelTXs = {tx->trunk(), tx->branch()};

      while (!currentLevelTXs.empty()) {
        auto removeTask =
            removeConfirmedTransactions(client, tips, currentLevelTXs)
                .then([&]() {
                  if (!currentLevelTXs.empty()) {
                    res.insert(currentLevelTXs.begin(), currentLevelTXs.end());
                    auto trytesVec =
                        clientSharedPtr->getTrytes(currentLevelTXs).get();
                    set<string> nextLevelTXs;  // Avoid duplications and allow a
                                               // minimal query for IRI

                    for_each(
                        trytesVec.begin(), trytesVec.end(),
                        [&nextLevelTXs](const string& trytes) {
                          nextLevelTXs.insert(trytes.substr(2430, 81).data());
                          nextLevelTXs.insert(trytes.substr(2511, 81).data());
                        });
                    currentLevelTXs.clear();
                    for_each(nextLevelTXs.begin(), nextLevelTXs.end(),
                             [&currentLevelTXs](const string& hash) {
                               currentLevelTXs.push_back(hash);
                             });
                  }
                });

        removeTask.wait();
      }
      return res;
    });
  }

  return pplx::create_task([]() -> set<string> { return set<string>(); });
}

pplx::task<std::optional<uint64_t>> handleUnseenTransactions(
    std::shared_ptr<iri::TXMessage> tx,
    cuckoohash_map<std::string, std::chrono::system_clock::time_point>&
        hashToDiscoveryTimestamp,
    std::chrono::time_point<std::chrono::system_clock> received,
    std::weak_ptr<api::IRIClient> iriClient) {
  TangleDB::TXRecord txRecord = {tx->hash(), tx->trunk(), tx->branch()};
  TangleDB::instance().put(std::move(txRecord));

  {
    std::chrono::system_clock::time_point txTime;
    if (hashToDiscoveryTimestamp.find(tx->hash(), txTime)) {
      auto txArrivalLatency =
          std::chrono::duration_cast<std::chrono::milliseconds>(received -
                                                                txTime)
              .count();
      hashToDiscoveryTimestamp.erase(tx->hash());
      return pplx::create_task(
          [txArrivalLatency = std::move(txArrivalLatency)]() -> std::optional<uint64_t> {
            return std::optional<uint64_t>(txArrivalLatency);
          });
    }
  }

  return getUnconfirmedTXs(iriClient, tx)
      .then([&](std::set<std::string> unconfirmed) {
        if (unconfirmed.empty()) {
          return std::optional<uint64_t>();
        }

        std::vector<std::string> unconfirmedVec(unconfirmed.begin(),
                                                unconfirmed.end());
        // remove seen transactions
        unconfirmedVec.erase(
            remove_if(unconfirmedVec.begin(), unconfirmedVec.end(),
                      [](std::string txHash) {
                        return TangleDB::instance().find(txHash).has_value();
                      }),
            unconfirmedVec.end());
        std::for_each(
            unconfirmedVec.begin(), unconfirmedVec.end(),
            [&hashToDiscoveryTimestamp, received](std::string txHash) {
              hashToDiscoveryTimestamp.insert(txHash, received);
            });
        return std::optional<uint64_t>();
      });
}
}  // namespace txAuxiliary
}  // namespace utils
}  // namespace iota
