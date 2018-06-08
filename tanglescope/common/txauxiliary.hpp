#pragma once

#include <iota/tanglescope/common/iri.hpp>
#include <libcuckoo/cuckoohash_map.hh>
#include <set>
#include <vector>

#include <boost/thread/executor.hpp>
#include <boost/thread/future.hpp>

#include "cppclient/beast.h"
/*
 * Under this namespace are functions to allow complex queries over IRI
 */

namespace iota {
namespace tanglescope {
namespace txAuxiliary {

boost::future<void> handleUnseenTransactions(
    std::shared_ptr<iri::TXMessage> tx,
    cuckoohash_map<std::string, std::chrono::system_clock::time_point>&
        hashToSeenTimestamp,
    std::chrono::time_point<std::chrono::system_clock> received,
    std::weak_ptr<cppclient::IotaAPI> iriClient, std::string lmhs);

std::set<std::string> getUnconfirmedTXs(
    std::weak_ptr<cppclient::IotaAPI> client,
    std::shared_ptr<iri::TXMessage> tx, std::string lmhs);

boost::future<void> removeConfirmedTransactions(
    std::weak_ptr<cppclient::IotaAPI> client,
    const std::vector<std::string>& tips, std::vector<std::string>& txs);

}  // namespace txAuxiliary
}  // namespace tanglescope
}  // namespace iota
