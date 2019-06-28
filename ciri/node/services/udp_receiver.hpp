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

class UdpReceiverService {
 public:
  UdpReceiverService(receiver_service_t* const service, boost::asio::io_context& context, uint16_t const port);
  ~UdpReceiverService();

 public:
  void receive();

 private:
  receiver_service_t* service_;
  boost::asio::ip::udp::socket socket_;
  boost::asio::ip::udp::endpoint senderEndpoint_;
  iota_packet_t packet_;
};
