#include "iota/tanglescope/tanglewidthcollector.hpp"
#include <glog/logging.h>
#include <iota/tanglescope/common/tangledb.hpp>
#include <map>

namespace iota {
namespace tanglescope {

std::map<std::string, std::string> TangleWidthCollector::nameToDescGauges = {
    {TangleWidthCollector::TANGLE_WIDTH,
     "Number of transactions approving a single transaction as observed "
     "across multiple nodes"}};

bool TangleWidthCollector::parseConfiguration(const YAML::Node& conf) {
  if (!PrometheusCollector::parseConfiguration(conf)) {
    return false;
  }

  if (!conf[SNAPSHOT_INTERVAL] || !conf[MEASURE_LINE_AGE]) {
    return false;
  }

  _snapshotInterval = conf[SNAPSHOT_INTERVAL].as<uint32_t>();
  _mesaureLineAge = conf[MEASURE_LINE_AGE].as<uint32_t>();

  return true;
}

void TangleWidthCollector::collect() {
  using namespace prometheus;
  VLOG(3) << __FUNCTION__;

  Exposer exposer{_prometheusExpURI};
  auto registry = std::make_shared<Registry>();
  exposer.RegisterCollectable(registry);

  _gauges =
      buildGaugeMap(registry, "tanglewidthcollector", {}, nameToDescGauges);

  analyzeWidthPeriodically();
}

void TangleWidthCollector::analyzeWidthPeriodically() {
  _collectorsThread = std::move(rxcpp::schedulers::make_current_thread());
  _collectorsWorker = _collectorsThread.create_worker();

  auto& thisRef = *this;
  if (_snapshotInterval > 0) {
    _collectorsWorker.schedule_periodically(
        _collectorsThread.now() + std::chrono::seconds(_mesaureLineAge * 3),
        std::chrono::seconds(_snapshotInterval),
        [&thisRef](auto scbl) { thisRef.analyzeWidthImpl(); });
  }
}

void TangleWidthCollector::analyzeWidthImpl() {
  /*******************************************************
   * Width definition (by Darcy Camargo):
   * W(s,t) = |{(Vi,Vj) : Vi approves Vj, Vi > s, Vj < s}|
   * 't' is of course > 's' and the more t > s the more
   * complete is the picture of the tangle in regards to
   * the measure line 's'
   *******************************************************/

  // has to be ordered
  std::map<uint64_t, uint32_t> offsetToWidth = {{0, 0},
                                                {MEASURE_LINE_AGE_STEP, 0},
                                                {2 * MEASURE_LINE_AGE_STEP, 0},
                                                {3 * MEASURE_LINE_AGE_STEP, 0},
                                                {4 * MEASURE_LINE_AGE_STEP, 0}};

  auto now = std::chrono::system_clock::now();
  auto measureLine =
      std::chrono::duration_cast<std::chrono::seconds>(now.time_since_epoch())
          .count() -
      _mesaureLineAge;

  for (auto& offWidthPair : offsetToWidth) {
    auto txs = std::move(TangleDB::instance().getTXsMap());
    for (auto&& kv : txs) {
      if (std::chrono::duration_cast<std::chrono::seconds>(
              kv.second.timestamp.time_since_epoch())
              .count() >= measureLine) {
        auto trunkIt = txs.find(kv.second.trunk);
        if (trunkIt != txs.end()) {
          if (std::chrono::duration_cast<std::chrono::seconds>(
                  trunkIt->second.timestamp.time_since_epoch())
                  .count() < measureLine) {
            ++offWidthPair.second;
          }
        }
        auto branchIt = txs.find(kv.second.branch);
        if (branchIt != txs.end()) {
          if (std::chrono::duration_cast<std::chrono::seconds>(
                  branchIt->second.timestamp.time_since_epoch())
                  .count() < measureLine) {
            ++offWidthPair.second;
          }
        }
      }
    }
    std::this_thread::sleep_for(std::chrono::seconds(MEASURE_LINE_AGE_STEP));
  }

  for (auto& offWidthPair : offsetToWidth) {
    _gauges.at(TANGLE_WIDTH)
        .get()
        .Add({{"time_after_measure_line",
               std::to_string(_mesaureLineAge + offWidthPair.first)}})
        .Set(offWidthPair.second);
  }
}

TangleWidthCollector::~TangleWidthCollector() {
  _collectorsWorker.unsubscribe();
}

}  // namespace tanglescope
}  // namespace iota
