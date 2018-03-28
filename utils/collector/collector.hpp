#pragma once

#include <string>
#include <yaml-cpp/yaml.h>

namespace iota {
    namespace utils {

        class CollectorBase {
        public:
            virtual ~CollectorBase();
            virtual void collect() = 0;
            virtual bool parseConfiguration(const YAML::Node& conf);
        };
    }
}

