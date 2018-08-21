/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#pragma once

#include <boost/asio.hpp>

#include "common/network/components/receiver.h"
#include "common/network/iota_packet.h"

class UdpReceiverService {
 public:
  UdpReceiverService(receiver_service_t* const service,
                     boost::asio::io_context& context, uint16_t const port);
  ~UdpReceiverService();

 public:
  void receive();
  bool handlePacket(std::size_t const length);

 private:
  receiver_service_t* service_;
  boost::asio::ip::udp::socket socket_;
  boost::asio::ip::udp::endpoint senderEndpoint_;
  iota_packet_t packet_;
};
