/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#include "../service.h"
#include "cclient/http/http.h"

#include <string>

#include <glog/logging.h>
#include <boost/asio/connect.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>
#include <boost/beast/version.hpp>

iota_api_result_t iota_service_query(const void* const service_opaque,
                                     char_buffer* obj, char_buffer* response) {
  using tcp = boost::asio::ip::tcp;
  namespace http = boost::beast::http;

  boost::asio::io_context ioc;
  boost::system::error_code ec;
  tcp::resolver resolver{ioc};
  tcp::socket socket{ioc};

  const iota_http_service_t* const service =
      reinterpret_cast<const iota_http_service_t* const>(service_opaque);

  iota_api_result_t result;
  result.is_error = RC_OK;

  try {
    auto const results =
        resolver.resolve(service->host, std::to_string(service->port));

    boost::asio::connect(socket, results.begin(), results.end());

    http::request<http::string_body> req{http::verb::post, "/", 11};
    req.set(http::field::host, service->host);
    req.set(http::field::user_agent, BOOST_BEAST_VERSION_STRING);
    req.set(http::field::content_type, service->content_type);
    req.set("X-IOTA-API-Version", service->version);
    req.body() = obj->data;
    req.content_length(req.body().size());

    VLOG(7) << __FUNCTION__ << " - req:\n" << req;

    http::write(socket, req);
    boost::beast::flat_buffer buffer;
    http::response<http::string_body> res;

    http::read(socket, buffer, res);
    result.is_error = char_buffer_allocate(response, res.body().size());
    if (result.is_error != RC_OK) return result;

    std::memcpy(response->data, res.body().data(), res.body().size());

    VLOG(7) << __FUNCTION__ << " - res:\n" << res;

    socket.shutdown(tcp::socket::shutdown_both, ec);

    if (ec && ec != boost::system::errc::not_connected)
      result.is_error = RC_CCLIENT_HTTP;
  } catch (const std::exception& ex) {
    result.is_error = RC_CCLIENT_HTTP;
  }

  return result;
}
