
#ifndef ENTANGLED_CPPCLIENT_IOTA_BEAST_H_
#define ENTANGLED_CPPCLIENT_IOTA_BEAST_H_

#include <cstdint>
#include <nonstd/optional.hpp>
#include <string>
#include <utility>
#include <vector>

#include <nlohmann/json_fwd.hpp>

#include "api.h"
#include "api_json.h"

namespace cppclient {
/// Implementation of IotaJsonAPI class. This is the actual IOTA API provider.
class BeastIotaAPI : virtual public IotaAPI, public IotaJsonAPI {
 public:
  BeastIotaAPI() = delete;
  BeastIotaAPI(std::string host, uint32_t port)
      : _host(std::move(host)), _port(port) {}
  virtual ~BeastIotaAPI() {}

 protected:
  nonstd::optional<nlohmann::json> post(const nlohmann::json& input) override;

 private:
  const std::string _host;
  const uint32_t _port;
};
}  // namespace cppclient

#endif  // ENTANGLED_CPPCLIENT_IOTA_BEAST_H_
