/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#include <boost/crc.hpp>
#include <iomanip>

#include "gossip/node.h"
#include "gossip/services/tcp_receiver.hpp"
#include "utils/logger_helper.h"

#define TCP_RECEIVER_SERVICE_LOGGER_ID "tcp_receiver_service"

static logger_id_t logger_id;

/*
 * TcpConnection
 */

TcpConnection::TcpConnection(receiver_service_t* const service,
                             boost::asio::ip::tcp::socket socket, uint16_t port)
    : service_(service), socket_(std::move(socket)), port_(port) {
  service->opaque_socket = &socket_;
}

TcpConnection::~TcpConnection() {
  log_info(logger_id, "Connection lost with node tcp://%s:%d\n",
           socket_.remote_endpoint().address().to_string().c_str(),
           socket_.remote_endpoint().port());
}

void TcpConnection::start() {
  char const* host = socket_.remote_endpoint().address().to_string().c_str();

  // Reading listening port from node

  std::array<char, PORT_SIZE> port_bytes;
  boost::system::error_code error;
  size_t len = socket_.read_some(boost::asio::buffer(port_bytes), error);
  if (len != PORT_SIZE) {
    log_warning(logger_id, "Received invalid port from node tcp://%s:%d\n",
                host, socket_.remote_endpoint().port());
    return;
  }
  uint16_t port = std::stoi(std::string(&port_bytes[0], len));

  // Looking for matching neighbor

  rw_lock_handle_wrlock(&service_->state->node->neighbors_lock);

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

  // Opening connection with neighbor

  boost::asio::ip::tcp::endpoint neighbor_endpoint(
      boost::asio::ip::address::from_string(host), port);
  auto neighbor_socket =
      new boost::asio::ip::tcp::socket(socket_.get_executor().context());
  neighbor_socket->connect(neighbor_endpoint);

  // Sending listening port to neighbor

  std::stringstream stream;
  stream << std::setfill('0') << std::setw(PORT_SIZE) << port_;
  boost::asio::write(*neighbor_socket, boost::asio::buffer(stream.str()),
                     error);

  neighbor->endpoint.opaque_inetaddr = neighbor_socket;

  rw_lock_handle_unlock(&service_->state->node->neighbors_lock);

  iota_packet_t packet;

  for (;;) {
    std::array<char, PACKET_SIZE + CRC_SIZE> tcp_packet;

    boost::asio::read(socket_, boost::asio::buffer(tcp_packet), error);

    if (error == boost::asio::error::eof) {
      break;
    } else if (error) {
      log_warning(logger_id,
                  "Reading from tethered node tcp://%s:%d failed: %s\n", host,
                  port, error.message().c_str());
      break;
    }

    // Computing CRC

    boost::crc_32_type result;
    result.process_bytes(&tcp_packet[0], PACKET_SIZE);
    stream.clear();
    stream.str(std::string());
    stream << std::setfill('0') << std::setw(CRC_SIZE) << std::hex
           << result.checksum();

    // Checking CRC

    if (stream.str() == std::string(&tcp_packet[0] + PACKET_SIZE, CRC_SIZE)) {
      memcpy(packet.content, &tcp_packet[0], PACKET_SIZE);
      iota_packet_set_endpoint(
          &packet, socket_.remote_endpoint().address().to_string().c_str(),
          port, PROTOCOL_TCP);
      processor_on_next(service_->processor, packet);
    }
  }
}

/*
 * TcpReceiverService
 */

TcpReceiverService::TcpReceiverService(receiver_service_t* const service,
                                       boost::asio::io_context& context,
                                       uint16_t const port)
    : service_(service),
      acceptor_(context, boost::asio::ip::tcp::endpoint(
                             boost::asio::ip::tcp::v4(), port)),
      port_(port) {
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
      std::make_shared<TcpConnection>(service_, std::move(socket), port_)
          ->start();
    }
    accept();
  });
}
