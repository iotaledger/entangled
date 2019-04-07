/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#include "cppclient/beast.h"

#include <nonstd/optional.hpp>

#include <glog/logging.h>
#include <boost/asio/connect.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>
#include <boost/beast/version.hpp>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

namespace {}  // namespace

namespace cppclient {
nonstd::optional<json> BeastIotaAPI::post(const json& input) {
  using tcp = boost::asio::ip::tcp;
  namespace http = boost::beast::http;

  boost::asio::io_context ioc;
  boost::system::error_code ec;
  tcp::resolver resolver{ioc};
  tcp::socket socket{ioc};

  json result;

  try {
    auto const results = resolver.resolve(_host, std::to_string(_port));

    boost::asio::connect(socket, results.begin(), results.end());

    http::request<http::string_body> req{http::verb::post, "/", 11};
    req.set(http::field::host, _host);
    req.set(http::field::user_agent, BOOST_BEAST_VERSION_STRING);
    req.set(http::field::content_type, "application/json");
    req.set("X-IOTA-API-Version", "1");
    req.body() = input.dump();
    req.content_length(req.body().size());

    VLOG(7) << __FUNCTION__ << " - req:\n" << req;

    http::write(socket, req);
    boost::beast::flat_buffer buffer;
    http::response<http::string_body> res;

    http::read(socket, buffer, res);
    result = json::parse(res.body());

    VLOG(7) << __FUNCTION__ << " - res:\n" << res;

    socket.shutdown(tcp::socket::shutdown_both, ec);

    if (ec && ec != boost::system::errc::not_connected) throw boost::system::system_error{ec};
  } catch (const std::exception& ex) {
    LOG(ERROR) << ex.what();
    return {};
  }

  return result;
}
}  // namespace cppclient
