/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#include "common/network/services/udp_receiver.hpp"

UdpReceiverService::UdpReceiverService(receiver_state_t* state,
                                       boost::asio::io_context& context,
                                       uint16_t const port)
    : state_(state),
      socket_(context, boost::asio::ip::udp::endpoint(
                           boost::asio::ip::udp::v4(), port)) {
  receive();
}

void UdpReceiverService::receive() {
  socket_.async_receive_from(
      boost::asio::buffer(packet_.content, TRANSACTION_PACKET_SIZE),
      senderEndpoint_,
      [this](boost::system::error_code ec, std::size_t length) {
        if (!ec && length > 0) {
          handlePacket(length);
        }
        receive();
      });
}

void UdpReceiverService::handlePacket(std::size_t const length) {
  // TODO(thibault) check size packet
  auto host = senderEndpoint_.address().to_string();
  memcpy(packet_.source.host, host.c_str(), host.size());
  packet_.source.host[host.size()] = '\0';
  packet_.source.port = senderEndpoint_.port();
  packet_.content[length] = '\0';
  packet_handler(state_, &packet_);
}
