#pragma once

#include <string>
#include <yaml-cpp/yaml.h>

namespace iota {
    namespace tanglescope {

        class CollectorBase {
        public:
            virtual ~CollectorBase();
            virtual void collect() = 0;
            virtual bool parseConfiguration(const YAML::Node& conf);
        };
    }
}

