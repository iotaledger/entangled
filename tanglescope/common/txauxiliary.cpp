#include "txauxiliary.hpp"

#include <set>

#include <gflags/gflags.h>
#include <glog/logging.h>
#include <iota/models/bundle.hpp>
#include <iota/models/transaction.hpp>

#include "common/helpers/digest.h"
#include "common/helpers/pow.h"
#include "common/trinary/tryte_long.h"
#include "tanglescope/common/tangledb.hpp"

DEFINE_string(transactionAddress,
              "JURSJVFIECKJYEHPATCXADQGHABKOOEZCRUHLIDHPNPIGRCXBFBWVISWCF9ODWQK"
              "LXBKY9FACCKVXRAGZ",
              "The Tryte's address of the transaction been generated by any of "
              "the Tanglescope's collectors");

namespace iota {
namespace tanglescope {
namespace txAuxiliary {

constexpr size_t FRAGMENT_LEN = 2187;
const std::string EMPTY_FRAG(FRAGMENT_LEN, '9');
const std::string EMPTY_NONCE(27, '9');
const std::string EMPTY_HASH(81, '9');

boost::future<void> removeConfirmedTransactions(std::weak_ptr<cppclient::IotaAPI> client,
                                                const std::vector<std::string>& tips, std::vector<std::string>& txs) {
  if (auto clientSharedPtr = client.lock()) {
    return boost::async(boost::launch::async, [clientSharedPtr = std::move(clientSharedPtr), txs = std::move(txs),
                                               tips] { return clientSharedPtr->getInclusionStates(txs, tips); })
        .then([&](auto future) {
          auto response = future.get();
          if (response.states.empty()) {
            txs.clear();
            return;
          }

          auto states = std::move(response.states);
          auto idx = 0;
          txs.erase(std::remove_if(txs.begin(), txs.end(), [&idx, &states](std::string hash) { return states[idx++]; }),
                    txs.end());
        });
  }
  return {};
}

std::set<std::string> getUnconfirmedTXs(std::weak_ptr<cppclient::IotaAPI> client, std::shared_ptr<iri::TXMessage> tx,
                                        std::string lmhs) {
  using namespace std;

  if (auto clientSharedPtr = client.lock()) {
    set<string> res;
    vector<string> currentLevelTXs = {tx->trunk(), tx->branch()};

    auto tips = {lmhs};
    while (!currentLevelTXs.empty()) {
      auto removeTask = removeConfirmedTransactions(client, tips, currentLevelTXs).then([&](auto future) {
        if (!currentLevelTXs.empty()) {
          res.insert(currentLevelTXs.begin(), currentLevelTXs.end());

          try {
            auto trytesVec = clientSharedPtr->getTrytes(currentLevelTXs);
            set<string> nextLevelTXs;  // Avoid duplications and allow a
            // minimal query for IRI

            for_each(trytesVec.begin(), trytesVec.end(), [&nextLevelTXs](const string& trytes) {
              nextLevelTXs.insert(trytes.substr(2430, 81).data());
              nextLevelTXs.insert(trytes.substr(2511, 81).data());
            });
            currentLevelTXs.clear();
            for_each(nextLevelTXs.begin(), nextLevelTXs.end(),
                     [&currentLevelTXs](const string& hash) { currentLevelTXs.push_back(hash); });
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
    return res;
  }

  return std::set<std::string>();
}

boost::future<void> handleUnseenTransactions(
    std::shared_ptr<iri::TXMessage> tx,
    cuckoohash_map<std::string, std::chrono::system_clock::time_point>& hashToDiscoveryTimestamp,
    std::chrono::time_point<std::chrono::system_clock> received, std::weak_ptr<cppclient::IotaAPI> iriClient,
    std::string lmhs) {
  TangleDB::TXRecord txRecord = {tx->hash(), tx->trunk(), tx->branch()};
  TangleDB::instance().put(std::move(txRecord));

  return boost::async(boost::launch::async, [tx, &hashToDiscoveryTimestamp, received = std::move(received), iriClient,
                                             lmhs = std::move(lmhs)]() {
    auto unconfirmed = getUnconfirmedTXs(iriClient, tx, std::move(lmhs));
    if (unconfirmed.empty()) {
      return;
    }

    std::vector<std::string> unconfirmedVec(unconfirmed.begin(), unconfirmed.end());
    // remove seen transactions
    unconfirmedVec.erase(remove_if(unconfirmedVec.begin(), unconfirmedVec.end(),
                                   [](std::string txHash) { return TangleDB::instance().find(txHash).has_value(); }),
                         unconfirmedVec.end());
    std::for_each(unconfirmedVec.begin(), unconfirmedVec.end(),
                  [&hashToDiscoveryTimestamp, received](std::string txHash) {
                    hashToDiscoveryTimestamp.insert(txHash, received);
                  });
    return;
  });
}

nonstd::optional<std::string> fillTX(
    boost::future<nonstd::optional<cppclient::GetTransactionsToApproveResponse>> fuResponse) {
  using namespace std::chrono;

  auto maybeResponse = std::move(fuResponse.get());
  if (!maybeResponse.has_value()) {
    return {};
  }
  auto response = std::move(maybeResponse.value());
  IOTA::Models::Transaction tx;
  IOTA::Models::Bundle bundle;

  tx.setAddress(FLAGS_transactionAddress);
  auto timestamp =
      std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now().time_since_epoch()).count();
  tx.setTrunkTransaction(response.trunkTransaction);
  tx.setBranchTransaction(response.branchTransaction);
  tx.setTimestamp(timestamp);
  tx.setValue(0);

  tx.setSignatureFragments(EMPTY_FRAG);
  tx.setNonce(EMPTY_NONCE);

  if (tx.getObsoleteTag().empty()) {
    tx.setObsoleteTag(EMPTY_NONCE);
  }

  bundle.addTransaction(tx, 1);
  bundle.finalize();

  return {std::move(bundle.getTransactions()[0].toTrytes())};
}

nonstd::optional<std::string> powTX(nonstd::optional<std::string> maybeTx, int mwm) {
  if (!maybeTx.has_value()) {
    return {};
  }

  auto tx = std::move(maybeTx.value());

  char* foundNonce = iota_pow_trytes(tx.data(), mwm);
  tx.replace(2646, 27, foundNonce);
  free(foundNonce);

  return {tx};
}

HashedTX hashTX(boost::future<nonstd::optional<std::string>> fuTx) {
  auto maybeResponse = std::move(fuTx.get());
  if (!maybeResponse.has_value()) {
    return {};
  }

  auto tx = std::move(maybeResponse.value());
  char* digest = iota_digest(tx.data());
  HashedTX hashed = {digest, std::move(tx)};
  free(digest);
  return hashed;
}
}  // namespace txAuxiliary
}  // namespace tanglescope
}  // namespace iota
