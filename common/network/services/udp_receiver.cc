/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#include "common/network/services/udp_receiver.hpp"
#include "ciri/node.h"
#include "common/network/neighbor.h"
#include "utils/logger_helper.h"

#define UDP_RECEIVER_SERVICE_LOGGER_ID "udp_receiver_service"

UdpReceiverService::UdpReceiverService(receiver_service_t* const service,
                                       boost::asio::io_context& context,
                                       uint16_t const port)
    : service_(service),
      socket_(context, boost::asio::ip::udp::endpoint(
                           boost::asio::ip::udp::v4(), port)) {
  logger_helper_init(UDP_RECEIVER_SERVICE_LOGGER_ID, LOGGER_DEBUG, true);
  receive();
}

UdpReceiverService::~UdpReceiverService() {
  logger_helper_destroy(UDP_RECEIVER_SERVICE_LOGGER_ID);
}

void UdpReceiverService::receive() {
  socket_.async_receive_from(
      boost::asio::buffer(packet_.content, TRANSACTION_PACKET_SIZE),
      senderEndpoint_,
      [this](boost::system::error_code ec, std::size_t length) {
        if (!ec && length > 0) {
          auto host = senderEndpoint_.address().to_string().c_str();
          auto port = senderEndpoint_.port();
          neighbor_t* neighbor = neighbor_find_by_values(
              service_->state->node->neighbors, PROTOCOL_UDP, host, port);
          if (neighbor == NULL) {
            log_debug(UDP_RECEIVER_SERVICE_LOGGER_ID,
                      "Packet denied from non-tethered neighbor udp://%s:%d\n",
                      host, port);
          } else {
            neighbor->endpoint.opaque_inetaddr = &socket_;
            handlePacket(length);
          }
        }
        receive();
      });
}

bool UdpReceiverService::handlePacket(std::size_t const length) {
  if (length != TRANSACTION_PACKET_SIZE) {
    return false;
  }
  receiver_service_prepare_packet(&packet_, length,
                                  senderEndpoint_.address().to_string().c_str(),
                                  senderEndpoint_.port(), PROTOCOL_UDP);
  log_debug(UDP_RECEIVER_SERVICE_LOGGER_ID,
            "Packet received from tethered neighbor udp://%s:%d\n",
            &packet_.source.host, packet_.source.port);
  if (service_->queue->vtable->push(service_->queue, packet_) !=
      CONCURRENT_QUEUE_SUCCESS) {
    return false;
  }
  return true;
}
