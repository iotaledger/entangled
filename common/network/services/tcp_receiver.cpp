/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#include <boost/bind.hpp>

#include "common/network/services/tcp_receiver.hpp"

/*
 * TcpConnection
 */

TcpConnection::TcpConnectionPtr TcpConnection::create(
    boost::asio::io_context& io_context) {
  return TcpConnectionPtr(new TcpConnection(io_context));
}

boost::asio::ip::tcp::socket& TcpConnection::socket() { return socket_; }

void TcpConnection::start() {
  message_ = "Hello";

  boost::asio::async_write(
      socket_, boost::asio::buffer(message_),
      boost::bind(&TcpConnection::handleWrite, shared_from_this()));
}

TcpConnection::TcpConnection(boost::asio::io_context& io_context)
    : socket_(io_context) {}

void TcpConnection::handleWrite() {}

/*
 * TcpReceiverService
 */

TcpReceiverService::TcpReceiverService(boost::asio::io_context& io_context,
                                       uint16_t const port)
    : acceptor_(io_context, boost::asio::ip::tcp::endpoint(
                                boost::asio::ip::tcp::v4(), port)) {
  startAccept();
}

void TcpReceiverService::startAccept() {
  TcpConnection::TcpConnectionPtr new_connection =
      TcpConnection::create(acceptor_.get_executor().context());

  acceptor_.async_accept(
      new_connection->socket(),
      boost::bind(&TcpReceiverService::handleAccept, this, new_connection,
                  boost::asio::placeholders::error));
}

void TcpReceiverService::handleAccept(
    TcpConnection::TcpConnectionPtr new_connection,
    const boost::system::error_code& error) {
  if (!error) {
    new_connection->start();
  }

  startAccept();
}
