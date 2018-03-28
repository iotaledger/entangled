#include <gflags/gflags.h>
#include <glog/logging.h>
#include <iota/utils/common/txauxiliary.hpp>
#include <iota/utils/echocatcher/echocatcher.hpp>
#include <iota/utils/statscollector.hpp>
#include <yaml-cpp/yaml.h>
#include <list>


DEFINE_string(ConfigurationPath, "", "YAML's configuration file path");


int main(int argc, char** argv) {

    ::gflags::ParseCommandLineFlags(&argc, &argv, true);
    ::google::InitGoogleLogging("tanglescope");

    std::list<pplx::task<void>> tasks;

    //need to parse yaml file and get arguments
    auto conf = YAML::LoadFile(FLAGS_ConfigurationPath.empty()?"default_configuration.yaml":FLAGS_ConfigurationPath);
    iota::utils::EchoCatcher echoCatcher;
    iota::utils::statscollector::StatsCollector statsCollector;

    if (echoCatcher.parseConfiguration(conf["echocatcher"])){
        auto task = pplx::task<void>([&echoCatcher](){echoCatcher.collect();});
        tasks.push_back(std::move(task));
    }

    if (statsCollector.parseConfiguration(conf["statscollector"])){
        auto task = pplx::task<void>([&statsCollector](){statsCollector.collect();});
        tasks.push_back(std::move(task));
    }

    std::for_each(tasks.begin(),tasks.end(),[](pplx::task<void>& task){task.wait();});

    return 0;
}
