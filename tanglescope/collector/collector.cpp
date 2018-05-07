#include <iota/tanglescope/collector/collector.hpp>

namespace iota {
    namespace tanglescope {
        CollectorBase::~CollectorBase(){

        }
        bool CollectorBase::parseConfiguration(const YAML::Node &conf) {
            if (!conf){
                return false;
            }
            return true;
        }
    }
}

