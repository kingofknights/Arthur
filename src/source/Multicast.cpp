//
// Created by VIKLOD on 26-02-2023.
//

#include "Multicast.hpp"

#include "DataFeed/CentralFeed.hpp"

#include <boost/asio/detail/socket_option.hpp>
#include <boost/asio/ip/address.hpp>
#include <boost/asio/ip/address_v4.hpp>
#include <boost/asio/ip/udp.hpp>
#include <boost/asio/socket_base.hpp>

#if WIN32
#include <Winsock2.h>
#include <Ws2tcpip.h>
#pragma comment(lib, WS2_32)
#endif

MulticastReceiver::MulticastReceiver(boost::asio::io_service& ioService_, MarketEventQueueT& queue_)
    : CentralFeed(queue_), _socket(ioService_) {
#if WIN32
    WSADATA wsaData;
    if (WSAStartup(0x0101, &wsaData)) {
        perror("WSAStartup");
    }
#endif
}

void MulticastReceiver::ReceiverFrom(const boost::system::error_code& errorCode_, size_t size_) {
    if (!errorCode_) {
        Process(size_);
        Read();
    }
}

void MulticastReceiver::BindMc(const std::string& address_, int port_, const std::string& multicast_) {
    _endpoint = boost::asio::ip::udp::endpoint(boost::asio::ip::address::from_string(address_), port_);
    _socket.open(_endpoint.protocol());
    _socket.set_option(boost::asio::socket_base::reuse_address(true));
    _socket.bind(_endpoint);
    _socket.set_option(boost::asio::ip::multicast::join_group(boost::asio::ip::address::from_string(multicast_)));
}

void MulticastReceiver::Read() {
    _socket.async_receive_from(boost::asio::buffer(_buffer, 512), _endpoint, [this](const boost::system::error_code& errorCode_, size_t size_) { ReceiverFrom(errorCode_, size_); });
}
