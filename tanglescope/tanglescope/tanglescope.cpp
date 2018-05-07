#include <gflags/gflags.h>
#include <glog/logging.h>
#include <iota/tanglescope/common/txauxiliary.hpp>
#include <iota/tanglescope/echocatcher/echocatcher.hpp>
#include <iota/tanglescope/statscollector.hpp>
#include <iota/tanglescope/blowballcollector.hpp>
#include <yaml-cpp/yaml.h>
#include <list>


DEFINE_string(ConfigurationPath, "", "YAML's configuration file path");


int main(int argc, char** argv) {

    ::gflags::ParseCommandLineFlags(&argc, &argv, true);
    ::google::InitGoogleLogging("tanglescope");

    std::list<pplx::task<void>> tasks;

    //need to parse yaml file and get arguments
    auto conf = YAML::LoadFile(FLAGS_ConfigurationPath.empty()?"default_configuration.yaml":FLAGS_ConfigurationPath);
    iota::tanglescope::EchoCatcher echoCatcher;
    iota::tanglescope::statscollector::StatsCollector statsCollector;
    iota::tanglescope::BlowballCollector blowballCollector;

    if (echoCatcher.parseConfiguration(conf["echocatcher"])){
        auto task = pplx::task<void>([&echoCatcher](){echoCatcher.collect();});
        tasks.push_back(std::move(task));
    }

    if (statsCollector.parseConfiguration(conf["statscollector"])){
        auto task = pplx::task<void>([&statsCollector](){statsCollector.collect();});
        tasks.push_back(std::move(task));
    }

    if (blowballCollector.parseConfiguration(conf["blowballcollector"])){
        auto task = pplx::task<void>([&blowballCollector](){blowballCollector.collect();});
        tasks.push_back(std::move(task));
    }

    std::for_each(tasks.begin(),tasks.end(),[](pplx::task<void>& task){task.wait();});

    return 0;
}
