/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#include "gossip/services/tcp_receiver.hpp"
#include "gossip/node.h"
#include "utils/logger_helper.h"

#define TCP_RECEIVER_SERVICE_LOGGER_ID "tcp_receiver_service"

static logger_id_t logger_id;

/*
 * TcpConnection
 */

TcpConnection::TcpConnection(receiver_service_t* const service,
                             boost::asio::ip::tcp::socket socket)
    : service_(service), socket_(std::move(socket)) {
  service->opaque_socket = &socket_;
}

TcpConnection::~TcpConnection() {
  log_info(logger_id, "Connection lost with node tcp://%s:%d\n",
           socket_.remote_endpoint().address().to_string().c_str(),
           socket_.remote_endpoint().port());
}

void TcpConnection::start() {
  auto host = socket_.remote_endpoint().address().to_string().c_str();
  auto port = socket_.remote_endpoint().port();

  rw_lock_handle_rdlock(&service_->state->node->neighbors_lock);

  neighbor_t* neighbor = neighbors_find_by_endpoint_values(
      service_->state->node->neighbors, host, port, PROTOCOL_TCP);

  if (neighbor == NULL) {
    log_info(logger_id,
             "Connection denied with non-tethered neighbor tcp://%s:%d\n", host,
             port);
    rw_lock_handle_unlock(&service_->state->node->neighbors_lock);
    return;
  }

  log_info(logger_id,
           "Connection accepted with tethered neighbor tcp://%s:%d\n", host,
           port);
  rw_lock_handle_unlock(&service_->state->node->neighbors_lock);

  receive();
}

void TcpConnection::receive() {
  auto self(shared_from_this());
  boost::asio::async_read(
      socket_, boost::asio::buffer(packet_.content, PACKET_SIZE),
      [this, self](boost::system::error_code ec, std::size_t length) {
        if (!ec && length == PACKET_SIZE) {
          iota_packet_set_endpoint(
              &packet_, socket_.remote_endpoint().address().to_string().c_str(),
              socket_.remote_endpoint().port(), PROTOCOL_TCP);
          processor_on_next(service_->processor, packet_);
        }
        receive();
      });
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
  logger_id =
      logger_helper_enable(TCP_RECEIVER_SERVICE_LOGGER_ID, LOGGER_DEBUG, true);
  acceptor_.set_option(boost::asio::socket_base::reuse_address(true));
  accept();
}

TcpReceiverService::~TcpReceiverService() { logger_helper_release(logger_id); }

void TcpReceiverService::accept() {
  acceptor_.async_accept([this](boost::system::error_code ec,
                                boost::asio::ip::tcp::socket socket) {
    if (!ec) {
      std::make_shared<TcpConnection>(service_, std::move(socket))->start();
    }
    accept();
  });
}
