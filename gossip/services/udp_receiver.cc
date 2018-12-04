/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#include "gossip/services/udp_receiver.hpp"
#include "gossip/node.h"
#include "utils/containers/lists/concurrent_list_neighbor.h"
#include "utils/logger_helper.h"

#define UDP_RECEIVER_SERVICE_LOGGER_ID "udp_receiver_service"

UdpReceiverService::UdpReceiverService(receiver_service_t* const service,
                                       boost::asio::io_context& context,
                                       uint16_t const port)
    : service_(service),
      socket_(context, boost::asio::ip::udp::endpoint(
                           boost::asio::ip::udp::v4(), port)) {
  logger_helper_init(UDP_RECEIVER_SERVICE_LOGGER_ID, LOGGER_DEBUG, true);
  service->opaque_socket = &socket_;
  receive();
}

UdpReceiverService::~UdpReceiverService() {
  logger_helper_destroy(UDP_RECEIVER_SERVICE_LOGGER_ID);
}

void UdpReceiverService::receive() {
  socket_.async_receive_from(
      boost::asio::buffer(packet_.content, PACKET_SIZE), senderEndpoint_,
      [this](boost::system::error_code ec, std::size_t length) {
        if (!ec && length > 0) {
          auto host = senderEndpoint_.address().to_string().c_str();
          auto port = senderEndpoint_.port();
          neighbor_t* neighbor = neighbor_find_by_endpoint_values(
              service_->state->node->neighbors, host, port, PROTOCOL_UDP);
          if (neighbor == NULL) {
            log_debug(UDP_RECEIVER_SERVICE_LOGGER_ID,
                      "Packet denied from non-tethered neighbor udp://%s:%d\n",
                      host, port);
          } else {
            handlePacket(&neighbor->endpoint, length);
          }
        }
        receive();
      });
}

bool UdpReceiverService::handlePacket(endpoint_t* const endpoint,
                                      std::size_t const length) {
  if (length != PACKET_SIZE) {
    return false;
  }
  iota_packet_set_endpoint(&packet_, endpoint->ip, endpoint->port,
                           PROTOCOL_UDP);
  log_debug(UDP_RECEIVER_SERVICE_LOGGER_ID,
            "Packet received from tethered neighbor udp://%s:%d\n",
            endpoint->host, endpoint->port);
  if (processor_on_next(service_->processor, packet_) != RC_OK) {
    return false;
  }
  return true;
}
