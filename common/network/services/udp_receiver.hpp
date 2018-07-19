/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#pragma once

#include <boost/asio.hpp>

#include "common/network/iota_packet.h"

class UdpReceiverService {
 public:
  UdpReceiverService(boost::asio::io_context& io_context, uint16_t const port);

 public:
  void receive();
  void handlePacket(std::size_t const length) const;

 private:
  boost::asio::ip::udp::socket socket_;
  boost::asio::ip::udp::endpoint sender_endpoint_;
  char packet_[TRANSACTION_PACKET_SIZE];
};
