/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#include "common/network/services/tcp_receiver.hpp"

/*
 * TcpConnection
 */

TcpConnection::TcpConnection(boost::asio::ip::tcp::socket socket)
    : socket_(std::move(socket)) {}

void TcpConnection::start() { receive(); }

void TcpConnection::receive() {
  socket_.async_read_some(
      boost::asio::buffer(data_, TRANSACTION_PACKET_SIZE),
      [this](boost::system::error_code ec, std::size_t length) {
        if (!ec) {
          handlePacket(length);
        }
        receive();
      });
}

void TcpConnection::handlePacket(std::size_t const length) const {
  // TODO(thibault) call receiver handle packet function
}

/*
 * TcpReceiverService
 */

TcpReceiverService::TcpReceiverService(boost::asio::io_context& io_context,
                                       uint16_t const port)
    : acceptor_(io_context, boost::asio::ip::tcp::endpoint(
                                boost::asio::ip::tcp::v4(), port)) {
  accept();
}

void TcpReceiverService::accept() {
  acceptor_.async_accept([this](boost::system::error_code ec,
                                boost::asio::ip::tcp::socket socket) {
    if (!ec) {
      std::make_shared<TcpConnection>(std::move(socket))->start();
    }
    accept();
  });
}
