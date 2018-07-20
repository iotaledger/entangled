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
      boost::asio::buffer(packet_, TRANSACTION_PACKET_SIZE), senderEndpoint_,
      [this](boost::system::error_code ec, std::size_t length) {
        if (!ec && length > 0) {
          handlePacket(length);
        }
        receive();
      });
}

void UdpReceiverService::handlePacket(std::size_t const length) const {
  // TODO(thibault) check size packet
  iota_packet_t packet;

  auto host = senderEndpoint_.address().to_string();
  memcpy(packet.source.host, host.c_str(), host.size());
  packet.source.host[host.size()] = '\0';
  packet.source.port = senderEndpoint_.port();
  memcpy(packet.content, packet_, length);
  packet_handler(state_, packet);
}
