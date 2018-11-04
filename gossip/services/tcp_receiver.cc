/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#include "gossip/services/tcp_receiver.hpp"
#include "gossip/node.h"
#include "utils/containers/lists/concurrent_list_neighbor.h"
#include "utils/logger_helper.h"

#define TCP_RECEIVER_SERVICE_LOGGER_ID "tcp_receiver_service"

/*
 * TcpConnection
 */

TcpConnection::TcpConnection(receiver_service_t* const service,
                             boost::asio::ip::tcp::socket socket)
    : service_(service), socket_(std::move(socket)) {
  service->opaque_socket = &socket_;
}

TcpConnection::~TcpConnection() {
  if (neighbor_ != NULL) {
    log_info(TCP_RECEIVER_SERVICE_LOGGER_ID,
             "Connection lost with tethered neighbor tcp://%s:%d\n",
             socket_.remote_endpoint().address().to_string().c_str(),
             socket_.remote_endpoint().port());
    neighbor_->endpoint.opaque_inetaddr = NULL;
  }
}

void TcpConnection::start() {
  auto host = socket_.remote_endpoint().address().to_string().c_str();
  auto port = socket_.remote_endpoint().port();

  neighbor_ = neighbor_find_by_endpoint_values(service_->state->node->neighbors,
                                               host, port, PROTOCOL_TCP);
  if (neighbor_ == NULL) {
    log_info(TCP_RECEIVER_SERVICE_LOGGER_ID,
             "Connection denied with non-tethered neighbor tcp://%s:%d\n", host,
             port);
    return;
  }
  log_info(TCP_RECEIVER_SERVICE_LOGGER_ID,
           "Connection accepted with tethered neighbor tcp://%s:%d\n", host,
           port);
  neighbor_->endpoint.opaque_inetaddr = &socket_;
  receive();
}

void TcpConnection::receive() {
  auto self(shared_from_this());
  boost::asio::async_read(
      socket_, boost::asio::buffer(packet_.content, PACKET_SIZE),
      [this, self](boost::system::error_code ec, std::size_t length) {
        if (!ec && length > 0) {
          handlePacket(length);
          receive();
        }
      });
}

bool TcpConnection::handlePacket(std::size_t const length) {
  if (length != PACKET_SIZE) {
    return false;
  }
  iota_packet_set_endpoint(&packet_, neighbor_->endpoint.ip,
                           neighbor_->endpoint.port, PROTOCOL_TCP);
  log_debug(TCP_RECEIVER_SERVICE_LOGGER_ID,
            "Packet received from tethered neighbor tcp://%s:%d\n",
            neighbor_->endpoint.host, neighbor_->endpoint.port);
  if (processor_on_next(service_->processor, packet_) != RC_OK) {
    return false;
  }
  return true;
}

/*
 * TcpReceiverService
 */

TcpReceiverService::TcpReceiverService(receiver_service_t* const service,
                                       boost::asio::io_context& context,
                                       uint16_t const port)
    : service_(service),
      acceptor_(context, boost::asio::ip::tcp::endpoint(
                             boost::asio::ip::tcp::v4(), port)) {
  logger_helper_init(TCP_RECEIVER_SERVICE_LOGGER_ID, LOGGER_DEBUG, true);
  acceptor_.set_option(boost::asio::socket_base::reuse_address(true));
  accept();
}

TcpReceiverService::~TcpReceiverService() {
  logger_helper_destroy(TCP_RECEIVER_SERVICE_LOGGER_ID);
}

void TcpReceiverService::accept() {
  acceptor_.async_accept([this](boost::system::error_code ec,
                                boost::asio::ip::tcp::socket socket) {
    if (!ec) {
      std::make_shared<TcpConnection>(service_, std::move(socket))->start();
    }
    accept();
  });
}
