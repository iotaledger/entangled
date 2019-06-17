#include "tanglescope/tanglewidthcollector.hpp"

#include <map>

#include <glog/logging.h>

#include "tanglescope/common/tangledb.hpp"

namespace iota {
namespace tanglescope {

std::map<std::string, std::string> TangleWidthCollector::nameToDescGauges = {
    {TangleWidthCollector::TANGLE_WIDTH, "Width/Number of edges crossing a measure line"},
    {TangleWidthCollector::MEASURE_LINE, "Measure line's timestamp"}};

bool TangleWidthCollector::parseConfiguration(const YAML::Node& conf) {
  if (!PrometheusCollector::parseConfiguration(conf)) {
    return false;
  }

  if (!conf[SNAPSHOT_INTERVAL] || !conf[MEASURE_LINE_BASE_AGE] || !conf[MEASURE_LINE_AGE_STEP] ||
      !conf[MEASURE_LINE_MAX_AGE]) {
    return false;
  }

  _snapshotInterval = conf[SNAPSHOT_INTERVAL].as<uint32_t>();
  _measureLineBaseAge = conf[MEASURE_LINE_BASE_AGE].as<uint32_t>();
  _measureLineMaxAge = conf[MEASURE_LINE_MAX_AGE].as<uint32_t>();
  _measureLineAgeStep = conf[MEASURE_LINE_AGE_STEP].as<uint32_t>();

  return true;
}

void TangleWidthCollector::collect() {
  using namespace prometheus;
  VLOG(3) << __FUNCTION__;

  Exposer exposer{_prometheusExpURI};
  auto registry = std::make_shared<Registry>();
  exposer.RegisterCollectable(registry);

  _gauges = buildGaugeMap(registry, "tanglewidthcollector", {}, nameToDescGauges);

  analyzeWidthPeriodically();
}

void TangleWidthCollector::analyzeWidthPeriodically() {
  _collectorThread = std::move(rxcpp::schedulers::make_current_thread());
  _collectorWorker = _collectorThread.create_worker();

  if (_snapshotInterval > 0) {
    _collectorWorker.schedule_periodically(_collectorThread.now() + std::chrono::seconds(_measureLineBaseAge * 3),
                                           std::chrono::seconds(_snapshotInterval), [this](auto) {
                                             auto task =
                                                 boost::async(boost::launch::async, [this]() { analyzeWidthImpl(); });
                                             _tasks.emplace_back(std::move(task));
                                           });
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
  std::map<uint64_t, uint32_t> offsetToWidth;

  auto now = std::chrono::system_clock::now();
  auto measureLine =
      std::chrono::duration_cast<std::chrono::seconds>(now.time_since_epoch()).count() - _measureLineBaseAge;

  for (uint16_t offset = 0; offset < _measureLineMaxAge; offset += _measureLineAgeStep) {
    auto txs = TangleDB::instance().getTXsMap();
    for (auto&& kv : txs) {
      if (std::chrono::duration_cast<std::chrono::seconds>(kv.second.timestamp.time_since_epoch()).count() >=
          measureLine) {
        auto trunkIt = txs.find(kv.second.trunk);
        if (trunkIt != txs.end()) {
          if (std::chrono::duration_cast<std::chrono::seconds>(trunkIt->second.timestamp.time_since_epoch()).count() <
              measureLine) {
            ++offsetToWidth[offset];
          }
        }
        auto branchIt = txs.find(kv.second.branch);
        if (branchIt != txs.end()) {
          if (std::chrono::duration_cast<std::chrono::seconds>(branchIt->second.timestamp.time_since_epoch()).count() <
              measureLine) {
            ++offsetToWidth[offset];
          }
        }
      }
    }
    std::this_thread::sleep_for(std::chrono::seconds(_measureLineAgeStep));
  }

  for (auto& offWidthPair : offsetToWidth) {
    _gauges.at(TANGLE_WIDTH)
        .get()
        .Add({{"time_after_measure_line", std::to_string(_measureLineBaseAge + offWidthPair.first)}})
        .Set(offWidthPair.second);
  }

  _gauges.at(MEASURE_LINE).get().Add({}).Set(measureLine);
}

TangleWidthCollector::~TangleWidthCollector() {
  _collectorWorker.unsubscribe();
  std::for_each(_tasks.begin(), _tasks.end(), [&](auto& task) { task.wait(); });
}

}  // namespace tanglescope
}  // namespace iota
