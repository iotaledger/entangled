/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#include "common/network/services/udp_receiver.hpp"

UdpReceiverService::UdpReceiverService(boost::asio::io_context& io_context,
                                       uint16_t const port)
    : socket_(io_context, boost::asio::ip::udp::endpoint(
                              boost::asio::ip::udp::v4(), port)) {
  receive();
}

void UdpReceiverService::receive() {
  socket_.async_receive_from(
      boost::asio::buffer(packet_, TRANSACTION_PACKET_SIZE), sender_endpoint_,
      [this](boost::system::error_code ec, std::size_t length) {
        if (!ec && length > 0) {
          handlePacket(length);
        }
        receive();
      });
}

void UdpReceiverService::handlePacket(std::size_t const length) const {
  // TODO(thibault) call receiver handle packet function
}
