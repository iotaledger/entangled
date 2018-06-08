
#include "txauxiliary.hpp"
#include <glog/logging.h>
#include <iota/tanglescope/common/tangledb.hpp>
#include <set>

namespace iota {
namespace tanglescope {
namespace txAuxiliary {

boost::future<void> removeConfirmedTransactions(
    std::weak_ptr<cppclient::IotaAPI> client,
    const std::vector<std::string>& tips, std::vector<std::string>& txs) {
  if (auto clientSharedPtr = client.lock()) {
    return boost::async(boost::launch::async,
                        [clientSharedPtr = std::move(clientSharedPtr),
                         txs = std::move(txs), tips] {
                          return clientSharedPtr->getInclusionStates(txs, tips);
                        })
        .then([&](auto future) {
          auto response = future.get();
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
  return {};
}

std::set<std::string> getUnconfirmedTXs(
    std::weak_ptr<cppclient::IotaAPI> client,
    std::shared_ptr<iri::TXMessage> tx, std::string lmhs) {
  using namespace std;

  if (auto clientSharedPtr = client.lock()) {
    set<string> res;
    vector<string> currentLevelTXs = {tx->trunk(), tx->branch()};

    auto tips = {lmhs};
    while (!currentLevelTXs.empty()) {
      auto removeTask =
          removeConfirmedTransactions(client, tips, currentLevelTXs)
              .then([&](auto future) {
                if (!currentLevelTXs.empty()) {
                  res.insert(currentLevelTXs.begin(), currentLevelTXs.end());

                  try {
                    auto trytesVec =
                        clientSharedPtr->getTrytes(currentLevelTXs);
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
                  } catch (const std::exception& e) {
                    LOG(ERROR) << " Exception: " << e.what();
                  }
                }
              });
      try {
        removeTask.wait();
      } catch (const std::exception& e) {
        LOG(ERROR) << " Exception: " << e.what();
      }
    }
    return std::move(res);
  }

  return std::set<std::string>();
}

boost::future<void> handleUnseenTransactions(
    std::shared_ptr<iri::TXMessage> tx,
    cuckoohash_map<std::string, std::chrono::system_clock::time_point>&
        hashToDiscoveryTimestamp,
    std::chrono::time_point<std::chrono::system_clock> received,
    std::weak_ptr<cppclient::IotaAPI> iriClient, std::string lmhs) {
  TangleDB::TXRecord txRecord = {tx->hash(), tx->trunk(), tx->branch()};
  TangleDB::instance().put(std::move(txRecord));

  return boost::async(boost::launch::async, [tx, &hashToDiscoveryTimestamp,
                                             received = std::move(received),
                                             iriClient,
                                             lmhs = std::move(lmhs)]() {
    auto unconfirmed = getUnconfirmedTXs(iriClient, tx, std::move(lmhs));
    if (unconfirmed.empty()) {
      return;
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
    std::for_each(unconfirmedVec.begin(), unconfirmedVec.end(),
                  [&hashToDiscoveryTimestamp, received](std::string txHash) {
                    hashToDiscoveryTimestamp.insert(txHash, received);
                  });
    return;
  });
}
}  // namespace txAuxiliary
}  // namespace tanglescope
}  // namespace iota
