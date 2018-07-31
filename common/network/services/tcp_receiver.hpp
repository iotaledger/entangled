/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#pragma once

#include <boost/asio.hpp>

#include "common/network/components/receiver.h"

class TcpConnection : public std::enable_shared_from_this<TcpConnection> {
 public:
  TcpConnection(boost::asio::ip::tcp::socket socket);

 public:
  void start();

 private:
  void receive();
  void handlePacket(std::size_t const length);

 private:
  boost::asio::ip::tcp::socket socket_;
  iota_packet_t packet_;
};

class TcpReceiverService {
 public:
  TcpReceiverService(boost::asio::io_context& context, uint16_t const port);

 private:
  void accept();

 private:
  boost::asio::ip::tcp::acceptor acceptor_;
};
