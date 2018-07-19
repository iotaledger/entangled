/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#include <memory>

#include <boost/asio.hpp>

#include "common/network/iota_packet.h"
#include "common/network/logger.h"
#include "common/network/services/receiver.h"

using boost::asio::ip::udp;

class UdpReceiverService {
 public:
  UdpReceiverService(boost::asio::io_context& io_context, uint16_t port)
      : socket_(io_context, udp::endpoint(udp::v4(), port)) {
    receive();
  }

  void receive() {
    socket_.async_receive_from(
        boost::asio::buffer(packet_, TRANSACTION_PACKET_SIZE), sender_endpoint_,
        [this](boost::system::error_code ec, std::size_t length) {
          if (!ec && length > 0) {
            handle_packet(length);
          }
          receive();
        });
  }

  void handle_packet(std::size_t length) {
    // TODO(thibault) call receiver handle packet function
  }

 private:
  udp::socket socket_;
  udp::endpoint sender_endpoint_;
  char packet_[TRANSACTION_PACKET_SIZE];
};

bool receiver_service_start(receiver_state_t* const state) {
  if (state == NULL) {
    return false;
  }
  try {
    boost::asio::io_context ctx;
    state->opaque_network = &ctx;
    // TODO(thibault) port as parameter
    UdpReceiverService udpService(ctx, 4242);
    ctx.run();
  } catch (std::exception const& e) {
    log_error("Receiver service failed to start: %s", e.what());
    return false;
  }
  return true;
}

bool receiver_service_stop(receiver_state_t* const state) {
  if (state == NULL) {
    return false;
  }
  try {
    auto ctx =
        reinterpret_cast<boost::asio::io_context*>(state->opaque_network);
    if (ctx == NULL) {
      log_error(
          "Receiver service failed to stop: invalid network configuration");
      return false;
    }
    ctx->stop();
  } catch (std::exception const& e) {
    log_error("Receiver service failed to stop: %s", e.what());
    return false;
  }
  return true;
}
