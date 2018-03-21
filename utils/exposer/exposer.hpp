#pragma once

#include <string>
#include <yaml-cpp/yaml.h>

namespace iota {
    namespace utils {

        class StatsExposer {
        public:
            virtual void expose() = 0;
            virtual bool parseConfiguration(const YAML::Node& conf);
        };
    }
}

