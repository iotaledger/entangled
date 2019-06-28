/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#include <iomanip>

#include <boost/crc.hpp>

#include "ciri/node/node.h"
#include "ciri/node/services/tcp_receiver.hpp"
#include "utils/logger_helper.h"

#define TCP_RECEIVER_SERVICE_LOGGER_ID "tcp_receiver_service"

static logger_id_t logger_id;

/*
 * TcpConnection
 */

TcpConnection::TcpConnection(receiver_service_t* const service, boost::asio::ip::tcp::socket socket)
    : service_(service), socket_(std::move(socket)) {}

TcpConnection::~TcpConnection() {
  socket_.close();
  log_info(logger_id, "Connection lost with tethered node tcp://%s:%d\n", remote_host_.c_str(), remote_port_);
}

void TcpConnection::start(uint16_t const port) {
  remote_host_ = socket_.remote_endpoint().address().to_string();

  // Reading listening port from node

  std::array<char, PORT_SIZE> port_bytes;
  boost::system::error_code error;
  size_t len = socket_.read_some(boost::asio::buffer(port_bytes), error);
  if (len != PORT_SIZE) {
    log_warning(logger_id, "Received invalid port from node tcp://%s:%d\n", remote_host_.c_str(),
                socket_.remote_endpoint().port());
    return;
  }
  remote_port_ = std::stoi(std::string(&port_bytes[0], len));

  // Looking for matching neighbor

  rw_lock_handle_wrlock(&service_->state->node->neighbors_lock);

  neighbor_t* neighbor = neighbors_find_by_endpoint_values(service_->state->node->neighbors, remote_host_.c_str(),
                                                           remote_port_, PROTOCOL_TCP);

  if (neighbor == NULL) {
    log_info(logger_id, "Connection denied with non-tethered neighbor tcp://%s:%d\n", remote_host_.c_str(),
             remote_port_);
    rw_lock_handle_unlock(&service_->state->node->neighbors_lock);
    return;
  }

  log_info(logger_id, "Connection accepted with tethered neighbor tcp://%s:%d\n", remote_host_.c_str(), remote_port_);

  // Opening connection with neighbor

  boost::asio::ip::tcp::endpoint neighbor_endpoint(boost::asio::ip::address::from_string(remote_host_), remote_port_);
  auto neighbor_socket = new boost::asio::ip::tcp::socket(socket_.get_executor().context());
  neighbor_socket->connect(neighbor_endpoint);

  // Sending listening port to neighbor

  char encoded_port[PORT_SIZE + 1];

  snprintf(encoded_port, PORT_SIZE + 1, "%0*d", PORT_SIZE, port);
  boost::asio::write(*neighbor_socket, boost::asio::buffer(encoded_port, PORT_SIZE), error);

  neighbor->endpoint.opaque_inetaddr = neighbor_socket;

  rw_lock_handle_unlock(&service_->state->node->neighbors_lock);

  iota_packet_t packet;
  boost::crc_32_type result;
  char crc[CRC_SIZE + 1];

  for (;;) {
    std::array<char, PACKET_SIZE + CRC_SIZE> tcp_packet;

    boost::asio::read(socket_, boost::asio::buffer(tcp_packet), error);

    if (error) {
      log_warning(logger_id, "Reading from tethered node tcp://%s:%d failed: %s\n", remote_host_.c_str(), remote_port_,
                  error.message().c_str());
      break;
    }

    // Computing CRC

    result.process_bytes(&tcp_packet[0], PACKET_SIZE);
    snprintf(crc, CRC_SIZE + 1, "%0*x", CRC_SIZE, result.checksum());
    result.reset();

    // Checking CRC

    if (memcmp(crc, &tcp_packet[0] + PACKET_SIZE, CRC_SIZE) == 0) {
      memcpy(packet.content, &tcp_packet[0], PACKET_SIZE);
      iota_packet_set_endpoint(&packet, socket_.remote_endpoint().address().to_string().c_str(), remote_port_,
                               PROTOCOL_TCP);
      processor_on_next(service_->processor, packet);
    }
  }
}

/*
 * TcpReceiverService
 */

TcpReceiverService::TcpReceiverService(receiver_service_t* const service, boost::asio::io_context& context,
                                       uint16_t const port)
    : service_(service), acceptor_(context, boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v4(), port)) {
  logger_id = logger_helper_enable(TCP_RECEIVER_SERVICE_LOGGER_ID, LOGGER_DEBUG, true);
  acceptor_.set_option(boost::asio::socket_base::reuse_address(true));
  accept(port);
}

TcpReceiverService::~TcpReceiverService() { logger_helper_release(logger_id); }

void TcpReceiverService::accept(uint16_t const port) {
  acceptor_.async_accept([this, port](boost::system::error_code ec, boost::asio::ip::tcp::socket socket) {
    if (!ec) {
      std::make_shared<TcpConnection>(service_, std::move(socket))->start(port);
    }
    accept(port);
  });
}
