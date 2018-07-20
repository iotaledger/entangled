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
  UdpReceiverService(receiver_state_t* state, boost::asio::io_context& context,
                     uint16_t const port);

 public:
  void receive();
  void handlePacket(std::size_t const length) const;

 private:
  receiver_state_t* state_;
  boost::asio::ip::udp::socket socket_;
  boost::asio::ip::udp::endpoint senderEndpoint_;
  char packet_[TRANSACTION_PACKET_SIZE + 1];
};
