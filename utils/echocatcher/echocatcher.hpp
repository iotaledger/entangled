#pragma once

#include <iota/utils/exposer/exposer.hpp>
#include <iota/utils/common/api.hpp>
#include <iota/utils/common/iri.hpp>
#include <list>
#include <string>
#include <chrono>
#include <rx.hpp>


namespace iota {
namespace utils {

class EchoCatcher : public StatsExposer {
 public:
  void expose() override;
  bool parseConfiguration(const YAML::Node& conf) override;

  struct HashedTX {
    std::string hash;
    std::string tx;
  };

 protected:  // gmock classes
  virtual void loadDB();
  virtual HashedTX broadcastTransactions();
  virtual void handleRecievedTransactions(
      HashedTX hashed,
      std::chrono::time_point<std::chrono::system_clock> start);

 private:
  //Configuration
  std::string _prometheusExpURI;
  std::string _iriHost;
  std::list<std::string> _zmqPublishers;
  uint32_t _tangleDBWarmupPeriod;
  uint32_t _mwm;
  //Others
  std::shared_ptr<iota::utils::api::IRIClient> _iriClient;
 rxcpp::observable<std::shared_ptr<iri::IRIMessage>> _zmqObservable;
};

}  // namespace utils
}  // namespace iota
