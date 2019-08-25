/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#ifndef CPPCLIENT_BEAST_H_
#define CPPCLIENT_BEAST_H_

#include <cstdint>
#include <nonstd/optional.hpp>
#include <string>
#include <utility>
#include <vector>

#include <boost/beast/http.hpp>
#include <nlohmann/json_fwd.hpp>

#include "cppclient/api.h"
#include "cppclient/api_json.h"

namespace cppclient {
/// Implementation of IotaJsonAPI class. This is the actual IOTA API provider.
class BeastIotaAPI : virtual public IotaAPI, public IotaJsonAPI {
 public:
  BeastIotaAPI() = delete;
  BeastIotaAPI(std::string host, uint32_t port, bool useSsl) : IotaAPI(useSsl), _host(std::move(host)), _port(port) {}
  virtual ~BeastIotaAPI() {}

 protected:
  nonstd::optional<nlohmann::json> post(const nlohmann::json& input) override;
  nonstd::optional<nlohmann::json> postSsl(const nlohmann::json& input);
  nonstd::optional<nlohmann::json> postPlainText(const nlohmann::json& input);

 private:
  boost::beast::http::request<boost::beast::http::string_body> setupRequest(const nlohmann::json& input) const;
  const std::string _host;
  const uint32_t _port;
};
}  // namespace cppclient

#endif  // CPPCLIENT_BEAST_H_
