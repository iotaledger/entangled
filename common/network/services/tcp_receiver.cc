/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#include "common/network/services/tcp_receiver.hpp"
#include "common/network/services/receiver.h"

/*
 * TcpConnection
 */

TcpConnection::TcpConnection(receiver_state_t* const state,
                             boost::asio::ip::tcp::socket socket)
    : state_(state), socket_(std::move(socket)) {}

void TcpConnection::start() { receive(); }

void TcpConnection::receive() {
  auto self(shared_from_this());
  socket_.async_read_some(
      boost::asio::buffer(packet_.content, TRANSACTION_PACKET_SIZE),
      [this, self](boost::system::error_code ec, std::size_t length) {
        if (ec == boost::asio::error::eof ||
            ec == boost::asio::error::connection_reset) {
          // TODO(thibault) disconnect
        } else if (!ec && length > 0) {
          handlePacket(length);
          receive();
        }
      });
}

void TcpConnection::handlePacket(std::size_t const length) {
  receiver_service_prepare_packet(
      &packet_, length, socket_.remote_endpoint().address().to_string().c_str(),
      socket_.remote_endpoint().port(), ENDPOINT_PROTOCOL_TCP);
  receiver_packet_handler(state_, &packet_);
}

/*
 * TcpReceiverService
 */

TcpReceiverService::TcpReceiverService(receiver_state_t* const state,
                                       boost::asio::io_context& context,
                                       uint16_t const port)
    : state_(state),
      acceptor_(context, boost::asio::ip::tcp::endpoint(
                             boost::asio::ip::tcp::v4(), port)) {
  acceptor_.set_option(boost::asio::socket_base::reuse_address(true));
  accept();
}

void TcpReceiverService::accept() {
  acceptor_.async_accept([this](boost::system::error_code ec,
                                boost::asio::ip::tcp::socket socket) {
    if (!ec) {
      std::make_shared<TcpConnection>(state_, std::move(socket))->start();
    }
    accept();
  });
}
