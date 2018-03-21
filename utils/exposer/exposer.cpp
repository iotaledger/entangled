#include <iota/utils/exposer/exposer.hpp>

namespace iota {
    namespace utils {
        StatsExposer::~StatsExposer(){

        }
        bool StatsExposer::parseConfiguration(const YAML::Node &conf) {
            if (!conf){
                return false;
            }
            return true;
        }
    }
}

