#include <gflags/gflags.h>
#include <glog/logging.h>
#include <yaml-cpp/yaml.h>
#include <boost/thread/executor.hpp>
#include <boost/thread/future.hpp>
#include <iota/tanglescope/blowballcollector.hpp>
#include <iota/tanglescope/common/txauxiliary.hpp>
#include <iota/tanglescope/echocatcher/echocatcher.hpp>
#include <iota/tanglescope/statscollector.hpp>
#include <iota/tanglescope/tipselectioncollector/tipselectioncollector.hpp>
#include <list>

DEFINE_string(ConfigurationPath, "", "YAML's configuration file path");

int main(int argc, char** argv) {
  ::gflags::ParseCommandLineFlags(&argc, &argv, true);
  ::google::InitGoogleLogging("tanglescope");

  std::list<boost::future<void>> tasks;

  // need to parse yaml file and get arguments
  try {
    auto conf = YAML::LoadFile(FLAGS_ConfigurationPath.empty()
                                   ? "default_configuration.yaml"
                                   : FLAGS_ConfigurationPath);
    iota::tanglescope::EchoCatcher echoCatcher;
    iota::tanglescope::statscollector::StatsCollector statsCollector;
    iota::tanglescope::BlowballCollector blowballCollector;
    iota::tanglescope::TipSelectionCollector tipSelectionCollector;

    if (echoCatcher.parseConfiguration(conf["echocatcher"])) {
      auto task = boost::async(boost::launch::async,
                               [&echoCatcher]() { echoCatcher.collect(); });
      tasks.push_back(std::move(task));
    }

    if (statsCollector.parseConfiguration(conf["statscollector"])) {
      auto task = boost::async(boost::launch::async, [&statsCollector]() {
        statsCollector.collect();
      });
      tasks.push_back(std::move(task));
    }

    if (blowballCollector.parseConfiguration(conf["blowballcollector"])) {
      auto task = boost::async(boost::launch::async, [&blowballCollector]() {
        blowballCollector.collect();
      });
      tasks.push_back(std::move(task));
    }

    if (tipSelectionCollector.parseConfiguration(
            conf["tipselectioncollector"])) {
      auto task = boost::async(
          boost::launch::async,
          [&tipSelectionCollector]() { tipSelectionCollector.collect(); });
      tasks.push_back(std::move(task));
    }

    std::for_each(tasks.begin(), tasks.end(), [](auto& task) { task.wait(); });
  } catch (const std::exception& e) {
    LOG(ERROR) << __FUNCTION__ << " Exception: " << e.what();
  }

  return 0;
}
