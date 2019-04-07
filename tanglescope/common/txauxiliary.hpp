#pragma once

#include <libcuckoo/cuckoohash_map.hh>
#include <set>
#include <vector>

#include <boost/thread/executor.hpp>
#include <boost/thread/future.hpp>

#include "cppclient/beast.h"
#include "tanglescope/common/iri.hpp"
/*
 * Under this namespace are functions to allow complex queries over IRI
 */

namespace iota {
namespace tanglescope {
namespace txAuxiliary {

struct HashedTX {
  std::string hash;
  std::string tx;
};

boost::future<void> handleUnseenTransactions(
    std::shared_ptr<iri::TXMessage> tx,
    cuckoohash_map<std::string, std::chrono::system_clock::time_point>& hashToSeenTimestamp,
    std::chrono::time_point<std::chrono::system_clock> received, std::weak_ptr<cppclient::IotaAPI> iriClient,
    std::string lmhs);

std::set<std::string> getUnconfirmedTXs(std::weak_ptr<cppclient::IotaAPI> client, std::shared_ptr<iri::TXMessage> tx,
                                        std::string lmhs);

boost::future<void> removeConfirmedTransactions(std::weak_ptr<cppclient::IotaAPI> client,
                                                const std::vector<std::string>& tips, std::vector<std::string>& txs);

nonstd::optional<std::string> fillTX(
    boost::future<nonstd::optional<cppclient::GetTransactionsToApproveResponse>> response);

nonstd::optional<std::string> powTX(nonstd::optional<std::string>, int mwm);

HashedTX hashTX(boost::future<nonstd::optional<std::string>> fuTx);

}  // namespace txAuxiliary
}  // namespace tanglescope
}  // namespace iota
