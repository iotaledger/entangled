#include <atomic>
#include <chrono>
#include <iostream>
#include <map>
#include <set>
#include <cstring>
#include <list>
#include <unordered_set>
#include <future>

#include <gflags/gflags.h>
#include <glog/logging.h>
#include <rx.hpp>

#include <ccurl/ccurl.h>
#include <iota/utils/common/api.hpp>
#include <iota/utils/common/iri.hpp>
#include <iota/utils/common/zmqpub.hpp>
#include <iota/utils/common/tangledb.h>


#include <prometheus/exposer.h>


DEFINE_string(zmqURL, "tcp://m5.iotaledger.net:5556",
              "URL of ZMQ publisher to connect to");

DEFINE_string(iriHost, "http://node02.iotatoken.nl:14265",
              "URL of IRI API to use");

DEFINE_string(prometheusExposerIP, "0.0.0.0:8080",
              "IP/Port that the Prometheus Exposer binds to");

DEFINE_int32(mwm, 14, "Minimum Weight Magnitude");

DEFINE_uint64(tangleDBWarmupPeriod, 6*60,
"interval to wait for TangleDB to load in seconds");

using namespace iota::utils;

const std::string TX_TRYTES =
    "99999999999999999999999999999999999999999999999999999999999999999999999999"
    "99999999999999999999999999999999999999999999999999999999999999999999999999"
    "99999999999999999999999999999999999999999999999999999999999999999999999999"
    "99999999999999999999999999999999999999999999999999999999999999999999999999"
    "99999999999999999999999999999999999999999999999999999999999999999999999999"
    "99999999999999999999999999999999999999999999999999999999999999999999999999"
    "99999999999999999999999999999999999999999999999999999999999999999999999999"
    "99999999999999999999999999999999999999999999999999999999999999999999999999"
    "99999999999999999999999999999999999999999999999999999999999999999999999999"
    "99999999999999999999999999999999999999999999999999999999999999999999999999"
    "99999999999999999999999999999999999999999999999999999999999999999999999999"
    "99999999999999999999999999999999999999999999999999999999999999999999999999"
    "99999999999999999999999999999999999999999999999999999999999999999999999999"
    "99999999999999999999999999999999999999999999999999999999999999999999999999"
    "99999999999999999999999999999999999999999999999999999999999999999999999999"
    "99999999999999999999999999999999999999999999999999999999999999999999999999"
    "99999999999999999999999999999999999999999999999999999999999999999999999999"
    "99999999999999999999999999999999999999999999999999999999999999999999999999"
    "99999999999999999999999999999999999999999999999999999999999999999999999999"
    "99999999999999999999999999999999999999999999999999999999999999999999999999"
    "99999999999999999999999999999999999999999999999999999999999999999999999999"
    "99999999999999999999999999999999999999999999999999999999999999999999999999"
    "99999999999999999999999999999999999999999999999999999999999999999999999999"
    "99999999999999999999999999999999999999999999999999999999999999999999999999"
    "99999999999999999999999999999999999999999999999999999999999999999999999999"
    "99999999999999999999999999999999999999999999999999999999999999999999999999"
    "99999999999999999999999999999999999999999999999999999999999999999999999999"
    "99999999999999999999999999999999999999999999999999999999999999999999999999"
    "99999999999999999999999999999999999999999999999999999999999999999999999999"
    "99999999999999999999999999999999999999999SAYHELLOTOECHOCATCHERECHOCATCHING"
    "SINCETWENTYSEVENTEENONTHEIOTATANGLE999999999999999999999999999999999999999"
    "9VD9999999999999999999999999JAKBHNJIE999999999999999999JURSJVFIECKJYEHPATC"
    "XADQGHABKOOEZCRUHLIDHPNPIGRCXBFBWVISWCF9ODWQKLXBKY9FACCKVXRAGZ999999999999"
    "99999999999999999999999999999999999999999999999999999999999999999999999999"
    "99999999999999999999999999999999999999999999999999999999999999999999999999"
    "99999999999999999999999999999999999999999999999999999999999999999999999999"
    "999999999";

struct HashedTX {
  std::string hash;
  std::string tx;
};

std::string fillTX(api::GetTransactionsToApproveResponse response) {
  std::string tx = TX_TRYTES;

  tx.replace(2430, 81, response.trunkTransaction);
  tx.replace(2511, 81, response.branchTransaction);

  return std::move(tx);
}

pplx::task<std::string> constructTXs(api::IRIClient* client) {
  return client->getTransactionsToApprove().then(fillTX);
}

std::string powTX(std::string tx, int mwm) { return ccurl_pow(tx.data(), mwm); }

HashedTX hashTX(std::string tx) {
  return {ccurl_digest_transaction(tx.data()), std::move(tx)};
}

bool removeConfirmedTransactions(std::shared_ptr<api::IRIClient> client,
                                 const std::vector<std::string>& tips, std::vector<std::string>& txs){

    using namespace std;

    auto inclusionStatesResponse = std::move(client->getInclusionStates(txs,tips).get());

    if (inclusionStatesResponse.states.empty()){
    return false;
    }
    auto states = std::move(inclusionStatesResponse.states);

    auto idx = 0;
    txs.erase(remove_if(txs.begin(),
                       txs.end(),
                       [&idx, &states](string hash){return states[idx++];}),
        txs.end());
    return true;
}

std::set<std::string> getUnconfirmedTXs(
    std::shared_ptr<api::IRIClient> client, std::shared_ptr<iri::TXMessage> tx) {

 using namespace std;
 auto nodeInfo = client->getNodeInfo().get();
 auto tips = {nodeInfo.latestMilestone};

 set<string> res;
 vector<string> currentLevelTXs = {tx->branch(),tx->trunk()};

 while(!currentLevelTXs.empty())
 {

    if (!removeConfirmedTransactions(client,tips,currentLevelTXs) || currentLevelTXs.empty()){
     break;
    }

   res.insert(currentLevelTXs.begin(),currentLevelTXs.end());

   auto trytesVec = client->getTrytes(currentLevelTXs).get();
   set<string> nextLevelTXs;//Avoid duplications and allow a minimal query for IRI

   for_each(trytesVec.begin(), trytesVec.end(),
                   [&nextLevelTXs](const string& trytes){
                       nextLevelTXs.insert(trytes.substr(2430,81).data());
                       nextLevelTXs.insert(trytes.substr(2511,81).data());
                   });
   currentLevelTXs.clear();
   for_each(nextLevelTXs.begin(),nextLevelTXs.end(),[&currentLevelTXs](const string& hash){
       currentLevelTXs.push_back(hash);});
 }

 return res;
}

void handleUnseenTransactions(std::shared_ptr<iri::TXMessage> tx,
                              std::map<std::string,std::time_t>& hashToSeenTimestamp,
                              std::chrono::time_point<std::chrono::system_clock> received,
                              std::shared_ptr<api::IRIClient> iriClient){

    static std::mutex mapMutex;

    TXRecord txRecord = {tx->hash().data(),tx->trunk().data(),tx->branch().data()};
    putInTangleDB(tx->hash().data(),std::move(txRecord));

    {
        std::lock_guard<std::mutex> lock(mapMutex);
        auto txTimeIt = hashToSeenTimestamp.find(tx->hash());
        if (txTimeIt != hashToSeenTimestamp.end()){
            auto txArrivalLatency = std::chrono::duration_cast<std::chrono::milliseconds>(
                    received - std::chrono::system_clock::from_time_t(txTimeIt->second)).count();
            hashToSeenTimestamp.erase(tx->hash().data());
            LOG(INFO) << "TX: "<<tx->hash()<<" arrived "<<txArrivalLatency<<" milliseconds after it was firstly discovered";
        }
    }

    auto unconfirmed = getUnconfirmedTXs(iriClient,tx);

    if (unconfirmed.empty()){
        return;
    }

    std::vector<std::string> unconfirmedVec(unconfirmed.begin(),unconfirmed.end());
    //remove seen transactions
    unconfirmedVec.erase(remove_if(unconfirmedVec.begin(),
                                   unconfirmedVec.end(),
                                   [](std::string txHash){
                                       return !std::string_view(findInTangleDB(txHash.data()).hash).empty();}),
                         unconfirmedVec.end());

    std::lock_guard<std::mutex> lock(mapMutex);
    std::for_each(unconfirmedVec.begin(),unconfirmedVec.end(),[&hashToSeenTimestamp, received](std::string txHash){
        hashToSeenTimestamp[txHash] = std::chrono::system_clock::to_time_t(received);
    });
}

int main(int argc, char** argv) {
  ::gflags::ParseCommandLineFlags(&argc, &argv, true);
  ::google::InitGoogleLogging("echocatcher");
  using namespace prometheus;

  LOG(INFO) << "Booting up.";
  ccurl_pow_init();

  LOG(INFO) << "IRI Host: " << FLAGS_iriHost;

  auto iriClient = std::make_shared<api::IRIClient>(FLAGS_iriHost);

  Exposer exposer{FLAGS_prometheusExposerIP};

  // create a metrics registry with component=main labels applied to all its
  // metrics
  auto registry = std::make_shared<Registry>();

  auto& gauge_received_family = BuildGauge()
            .Name("time_elapsed_received")
            .Help("#Milli seconds it took for tx to travel back to transaction's original source(\"listen_node\")")
            .Labels({{"publish_node",FLAGS_zmqURL},{"listen_node",FLAGS_iriHost}})
            .Register(*registry);

  auto& gauge_arrived_family = BuildGauge()
            .Name("time_elapsed_arrived")
            .Help("#Milli seconds it took for tx to arrive to destination (\"publish_node\")")
            .Labels({{"publish_node",FLAGS_zmqURL},{"listen_node",FLAGS_iriHost}})
            .Register(*registry);

  exposer.RegisterCollectable(registry);

  std::atomic<bool> haveAllTXReturned = false;
  auto tangleDBWarmupPeriod = std::chrono::milliseconds(FLAGS_tangleDBWarmupPeriod*1000);

  std::map<std::string,std::time_t> hashToSeenTimestamp;
  std::vector<std::shared_future<void>> futures;

  auto zmqThread = rxcpp::schedulers::make_new_thread();

  auto zmqObservable =
      rxcpp::observable<>::create<std::shared_ptr<iri::IRIMessage>>(
          [&](auto s) { zmqPublisher(std::move(s), FLAGS_zmqURL, haveAllTXReturned); })
          .observe_on(rxcpp::observe_on_new_thread());

  auto stopPopulatingDBTime =
          std::chrono::system_clock::now() + tangleDBWarmupPeriod;

  zmqObservable.observe_on(rxcpp::synchronize_new_thread())
          .take_while([stopPopulatingDBTime](std::shared_ptr<iri::IRIMessage> msg){
              return std::chrono::system_clock::now() < stopPopulatingDBTime;
          })
          .subscribe(
              [](std::shared_ptr<iri::IRIMessage> msg) {
                  if (msg->type() != iri::IRIMessageType::TX) return;

                  auto tx = std::static_pointer_cast<iri::TXMessage>(std::move(msg));

                  TXRecord txRec = {tx->hash().data(),tx->trunk().data(),tx->branch().data()};
                  putInTangleDB(tx->hash().data(),std::move(txRec));
              },
              []() {});

  auto task =
        iriClient->getTransactionsToApprove()
                .then(fillTX)
                .then([](std::string tx) { return powTX(std::move(tx), FLAGS_mwm); })
                .then(hashTX);

  task.wait();
  auto hashed = task.get();
  LOG(INFO) << "Hash: " << hashed.hash;

  auto broadcast = iriClient->broadcastTransactions({hashed.tx});
  auto start = std::chrono::system_clock::now();

  zmqObservable.observe_on(rxcpp::synchronize_new_thread())
      .subscribe(
          [start, hashed, &gauge_received_family,
           &gauge_arrived_family, &haveAllTXReturned, &iriClient,
           &hashToSeenTimestamp, &futures](std::shared_ptr<iri::IRIMessage> msg) {
            if (msg->type() != iri::IRIMessageType::TX) return;

            auto tx = std::static_pointer_cast<iri::TXMessage>(std::move(msg));

              auto received = std::chrono::system_clock::now();

              futures.push_back(std::async(std::launch::async, handleUnseenTransactions,tx, std::ref(hashToSeenTimestamp), received, iriClient));

            if (tx->hash() == hashed.hash) {
              auto& current_received_duration_gauge = gauge_received_family.Add(
                  {{"bundle_size", std::to_string(tx->lastIndex() + 1)}});

              auto& current_arrived_duration_gauge = gauge_arrived_family.Add(
                  {{"bundle_size", std::to_string(tx->lastIndex() + 1)}});


              auto elapsed_until_received =
                  std::chrono::duration_cast<std::chrono::milliseconds>(
                      received - start)
                      .count();

              auto elapsed_until_arrived =
                  std::chrono::duration_cast<std::chrono::milliseconds>(
                      tx->arrivalTime() - start)
                      .count();

              current_received_duration_gauge.Set(elapsed_until_received);
              current_arrived_duration_gauge.Set(elapsed_until_arrived);
              haveAllTXReturned = true;

            }
          },
          []() {});

    std::for_each(futures.begin(),futures.end(),[](std::shared_future<void>& fu){if (fu.valid())fu.get();});


  return 0;
}
