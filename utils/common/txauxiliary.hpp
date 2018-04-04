#pragma once

#include <iota/utils/common/api.hpp>
#include <iota/utils/common/iri.hpp>
#include <libcuckoo/cuckoohash_map.hh>
#include <set>

/*
 * Under this namespace are functions to allow complex queries over IRI
 */

namespace iota {
namespace utils {
namespace txAuxiliary {

pplx::task<void> handleUnseenTransactions(
    std::shared_ptr<iri::TXMessage> tx,
    cuckoohash_map<std::string, std::chrono::system_clock::time_point>&
        hashToSeenTimestamp,
    std::chrono::time_point<std::chrono::system_clock> received,
    std::weak_ptr<api::IRIClient> iriClient);

pplx::task<std::set<std::string>> getUnconfirmedTXs(
    std::weak_ptr<api::IRIClient> client, std::shared_ptr<iri::TXMessage> tx);

pplx::task<void> removeConfirmedTransactions(
    std::weak_ptr<api::IRIClient> client, const std::vector<std::string>& tips,
    std::vector<std::string>& txs);

}  // namespace txAuxiliary
}  // namespace utils
}  // namespace iota
