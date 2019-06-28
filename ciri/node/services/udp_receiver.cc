/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#include "ciri/node/services/udp_receiver.hpp"
#include "ciri/node/services/receiver.h"

UdpReceiverService::UdpReceiverService(receiver_service_t* const service, boost::asio::io_context& context,
                                       uint16_t const port)
    : service_(service), socket_(context, boost::asio::ip::udp::endpoint(boost::asio::ip::udp::v4(), port)) {
  service->opaque_socket = &socket_;
  receive();
}

UdpReceiverService::~UdpReceiverService() {}

void UdpReceiverService::receive() {
  socket_.async_receive_from(boost::asio::buffer(packet_.content, PACKET_SIZE), senderEndpoint_,
                             [this](boost::system::error_code ec, std::size_t length) {
                               if (!ec && length == PACKET_SIZE) {
                                 iota_packet_set_endpoint(&packet_, senderEndpoint_.address().to_string().c_str(),
                                                          senderEndpoint_.port(), PROTOCOL_UDP);
                                 processor_on_next(service_->processor, packet_);
                               }
                               receive();
                             });
}
