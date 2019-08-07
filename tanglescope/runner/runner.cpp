#include <list>

#include <gflags/gflags.h>
#include <glog/logging.h>
#include <yaml-cpp/yaml.h>
#include <boost/thread/executor.hpp>
#include <boost/thread/future.hpp>

#include "tanglescope/blowballcollector.hpp"
#include "tanglescope/common/txauxiliary.hpp"
#include "tanglescope/common/zmqdbloader.hpp"
#include "tanglescope/confirmationratecollector.hpp"
#include "tanglescope/echocollector.hpp"
#include "tanglescope/statscollector/statscollector.hpp"
#include "tanglescope/tanglewidthcollector.hpp"
#include "tanglescope/tipselectioncollector.hpp"

DEFINE_string(ConfigurationPath, "", "YAML's configuration file path");

int main(int argc, char** argv) {
  ::gflags::ParseCommandLineFlags(&argc, &argv, true);
  ::google::InitGoogleLogging("tanglescope");

  std::list<boost::future<void>> tasks;
  printf("\tWorkdir: %s\n", getenv("PWD"));
  // need to parse yaml file and get arguments
  try {
    auto conf =
        YAML::LoadFile(FLAGS_ConfigurationPath.empty() ? "default_configuration.yaml" : FLAGS_ConfigurationPath);
    iota::tanglescope::EchoCollector echoCatcher;
    iota::tanglescope::statscollector::StatsCollector statsCollector;
    iota::tanglescope::BlowballCollector blowballCollector;
    iota::tanglescope::TipSelectionCollector tipSelectionCollector;
    iota::tanglescope::TangleWidthCollector widthCollector;
    iota::tanglescope::CRCollector crCollector;
    iota::tanglescope::ZmqDBLoader dbLoader;

    if (conf["echocollector"] || conf["tanglewidthcollector"]) {
      if (!dbLoader.parseConfiguration(conf["dbloader"])) {
        LOG(FATAL) << __FUNCTION__
                   << ": Failed to parse dbloader (required for "
                      "echocollector and tanglewidthcollector)";
      } else {
        auto task = boost::async(boost::launch::async, [&dbLoader]() { dbLoader.start(); });
        tasks.emplace_back(std::move(task));
      }
    }

    if (echoCatcher.parseConfiguration(conf["echocollector"])) {
      auto task = boost::async(boost::launch::async, [&echoCatcher]() { echoCatcher.collect(); });
      tasks.emplace_back(std::move(task));
    }

    if (statsCollector.parseConfiguration(conf["statscollector"])) {
      auto task = boost::async(boost::launch::async, [&statsCollector]() { statsCollector.collect(); });
      tasks.emplace_back(std::move(task));
    }

    if (blowballCollector.parseConfiguration(conf["blowballcollector"])) {
      auto task = boost::async(boost::launch::async, [&blowballCollector]() { blowballCollector.collect(); });
      tasks.emplace_back(std::move(task));
    }

    if (tipSelectionCollector.parseConfiguration(conf["tipselectioncollector"])) {
      auto task = boost::async(boost::launch::async, [&tipSelectionCollector]() { tipSelectionCollector.collect(); });
      tasks.emplace_back(std::move(task));
    }

    if (widthCollector.parseConfiguration(conf["tanglewidthcollector"])) {
      auto task = boost::async(boost::launch::async, [&widthCollector]() { widthCollector.collect(); });
      tasks.emplace_back(std::move(task));
    }

    if (crCollector.parseConfiguration(conf["confirmationratecollector"])) {
      auto task = boost::async(boost::launch::async, [&crCollector]() { crCollector.collect(); });
      tasks.emplace_back(std::move(task));
    }

    std::for_each(tasks.begin(), tasks.end(), [](auto& task) { task.wait(); });
  } catch (const std::exception& e) {
    LOG(ERROR) << __FUNCTION__ << " Exception: " << e.what();
  }

  return 0;
}
