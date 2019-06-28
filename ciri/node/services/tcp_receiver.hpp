/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#pragma once

#include <boost/asio.hpp>

#include "ciri/node/iota_packet.h"

// Forward declarations
typedef struct receiver_service_s receiver_service_t;
typedef struct neighbor_s neighbor_t;

class TcpConnection : public std::enable_shared_from_this<TcpConnection> {
 public:
  TcpConnection(receiver_service_t* const service, boost::asio::ip::tcp::socket socket);
  ~TcpConnection();

 public:
  void start(uint16_t const port);

 private:
  receiver_service_t* service_;
  boost::asio::ip::tcp::socket socket_;
  std::string remote_host_;
  uint16_t remote_port_;
};

class TcpReceiverService {
 public:
  TcpReceiverService(receiver_service_t* const service, boost::asio::io_context& context, uint16_t const port);
  ~TcpReceiverService();

 private:
  void accept(uint16_t const port);

 private:
  receiver_service_t* service_;
  boost::asio::ip::tcp::acceptor acceptor_;
};
