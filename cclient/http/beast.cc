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

retcode_t iota_service_query(const void* const service_opaque,
                             char_buffer_t* obj, char_buffer_t* response) {
  using tcp = boost::asio::ip::tcp;
  namespace http = boost::beast::http;

  boost::asio::io_context ioc;
  boost::system::error_code ec;
  tcp::resolver resolver{ioc};
  tcp::socket socket{ioc};

  const iota_client_service_t* const service =
      reinterpret_cast<const iota_client_service_t* const>(service_opaque);
  const http_info_t* http_setting = &service->http;

  retcode_t result = RC_OK;

  try {
    auto const results = resolver.resolve(http_setting->host,
                                          std::to_string(http_setting->port));

    boost::asio::connect(socket, results.begin(), results.end());

    http::request<http::string_body> req{http::verb::post, "/", 11};
    req.set(http::field::host, http_setting->host);
    req.set(http::field::user_agent, BOOST_BEAST_VERSION_STRING);
    req.set(http::field::content_type, "application/json");
    req.set("X-IOTA-API-Version", http_setting->api_version);
    req.body() = obj->data;
    req.content_length(req.body().size());

    VLOG(7) << __FUNCTION__ << " - req:\n" << req;

    http::write(socket, req);
    boost::beast::flat_buffer buffer;
    http::response<http::string_body> res;

    http::read(socket, buffer, res);
    result = char_buffer_allocate(response, res.body().size());
    if (result != RC_OK) {
      return result;
    }

    std::memcpy(response->data, res.body().data(), res.body().size());

    VLOG(7) << __FUNCTION__ << " - res:\n" << res;

    socket.shutdown(tcp::socket::shutdown_both, ec);

    if (ec && ec != boost::system::errc::not_connected)
      result = RC_CCLIENT_HTTP_REQ;
  } catch (const std::exception& ex) {
    result = RC_CCLIENT_HTTP_REQ;
  }

  return result;
}
