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
#include <boost/asio/ssl/error.hpp>
#include <boost/asio/ssl/stream.hpp>
#include <boost/beast/core.hpp>
#include <boost/beast/ssl.hpp>
#include <boost/beast/version.hpp>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

namespace beast = boost::beast;  // from <boost/beast.hpp>
namespace http = beast::http;    // from <boost/beast/http.hpp>
namespace net = boost::asio;     // from <boost/asio.hpp>
namespace ssl = net::ssl;        // from <boost/asio/ssl.hpp>
using tcp = net::ip::tcp;        // from <boost/asio/ip/tcp.hpp>

namespace cppclient {

nonstd::optional<json> BeastIotaAPI::post(const json& input) {
  nonstd::optional<json> result;

  try {
    if (isSsl()) {
      result = post_ssl(input);
    } else {
      result = post_plain(input);
    }
  } catch (const std::exception& ex) {
    LOG(ERROR) << ex.what();
    return {};
  }

  return result;
}

nonstd::optional<json> BeastIotaAPI::post_plain(const json& input) {
  net::io_context ioc;
  boost::system::error_code ec;
  tcp::resolver resolver{ioc};
  tcp::socket socket{ioc};

  json result;

  try {
    auto const results = resolver.resolve(_host, std::to_string(_port));

    net::connect(socket, results.begin(), results.end());

    http::request<http::string_body> req = setupRequest(input);

    VLOG(7) << __FUNCTION__ << " - req:\n" << req;

    http::write(socket, req);
    beast::flat_buffer buffer;
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

nonstd::optional<json> BeastIotaAPI::post_ssl(const json& input) {
  net::io_context ioc;
  boost::system::error_code ec;
  tcp::resolver resolver{ioc};

  json result;

  try {
    // The SSL context is required, and holds certificates
    ssl::context ctx(ssl::context::tlsv12_client);

    // Verify the remote server's certificate
    ctx.set_verify_mode(ssl::verify_peer);
    if (!sslPemPath().empty()) {
      ctx.load_verify_file(sslPemPath());
    }

    beast::ssl_stream<beast::tcp_stream> stream(ioc, ctx);

    // Set SNI Hostname (many hosts need this to handshake successfully)
    if (!SSL_set_tlsext_host_name(stream.native_handle(), _host.c_str())) {
      beast::error_code ec{static_cast<int>(::ERR_get_error()), net::error::get_ssl_category()};
      throw beast::system_error{ec};
    }
    auto const results = resolver.resolve(_host, std::to_string(_port));

    beast::get_lowest_layer(stream).connect(results);

    // Perform the SSL handshake
    stream.handshake(ssl::stream_base::client);

    http::request<http::string_body> req = setupRequest(input);

    VLOG(7) << __FUNCTION__ << " - req:\n" << req;

    http::write(stream, req);
    beast::flat_buffer buffer;
    http::response<http::string_body> res;

    http::read(stream, buffer, res);
    result = json::parse(res.body());

    VLOG(7) << __FUNCTION__ << " - res:\n" << res;

    stream.shutdown(ec);

    if (ec && ec != boost::system::errc::not_connected) throw boost::system::system_error{ec};
  } catch (const std::exception& ex) {
    LOG(ERROR) << ex.what();
    return {};
  }

  return result;
}

http::request<http::string_body> BeastIotaAPI::setupRequest(const nlohmann::json& input) const {
  http::request<http::string_body> req{http::verb::post, "/", 11};
  req.set(http::field::host, _host);
  req.set(http::field::user_agent, BOOST_BEAST_VERSION_STRING);
  req.set(http::field::content_type, "application/json");
  req.set("X-IOTA-API-Version", "1");
  req.body() = input.dump();
  req.content_length(req.body().size());
  return std::move(req);
}
}  // namespace cppclient
