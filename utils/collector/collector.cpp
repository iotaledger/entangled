#include <iota/utils/collector/collector.hpp>

namespace iota {
    namespace utils {
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

