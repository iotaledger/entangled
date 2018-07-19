/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#pragma once

#include <boost/asio.hpp>

#include "common/network/iota_packet.h"

class TcpConnection : public std::enable_shared_from_this<TcpConnection> {
 public:
  using TcpConnectionPtr = std::shared_ptr<TcpConnection>;

 public:
  static TcpConnectionPtr create(boost::asio::io_context& io_context);
  boost::asio::ip::tcp::socket& socket();
  void start();

 private:
  TcpConnection(boost::asio::io_context& io_context);
  void handleWrite();

 private:
  boost::asio::ip::tcp::socket socket_;
  std::string message_;
};

class TcpReceiverService {
 public:
  TcpReceiverService(boost::asio::io_context& io_context, uint16_t const port);

 private:
  void startAccept();
  void handleAccept(TcpConnection::TcpConnectionPtr new_connection,
                    const boost::system::error_code& error);

 private:
  boost::asio::ip::tcp::acceptor acceptor_;
};
