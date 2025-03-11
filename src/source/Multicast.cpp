//
// Created by VIKLOD on 26-02-2023.
//

#include "../include/Multicast.hpp"
#include <asm-generic/socket.h>
#include <boost/asio/detail/socket_option.hpp>
#include <boost/asio/ip/address.hpp>
#include <boost/asio/ip/address_v4.hpp>
#include <boost/asio/ip/udp.hpp>
#include <boost/asio/socket_base.hpp>
#include <iostream>

MulticastReceiver::MulticastReceiver(boost::asio::io_service& ioService_) : _socket(ioService_) {
}

void MulticastReceiver::receiverFrom(const boost::system::error_code& errorCode_, size_t size_) {
    if (!errorCode_) {
        processData(_buffer, size_);
        read();
    }
}

void MulticastReceiver::bindMC(const std::string& address_, int port_) {
    std::cout << __PRETTY_FUNCTION__ << std::endl;
    _endpoint = boost::asio::ip::udp::endpoint(boost::asio::ip::udp::v4(), port_);
    _socket.open(boost::asio::ip::udp::v4());
    _socket.set_option(boost::asio::socket_base::reuse_address(true));
    _socket.set_option(boost::asio::detail::socket_option::boolean<SOL_SOCKET, SO_REUSEPORT>(true));
    _socket.bind(_endpoint);
}

void MulticastReceiver::read() {
    _socket.async_receive_from(boost::asio::buffer(_buffer, MAX_LENGTH), _endpoint, [this](const boost::system::error_code& errorCode_, size_t size_) { receiverFrom(errorCode_, size_); });
}

void MulticastReceiver::processData(char* data_, size_t size_) { Process(data_, size_); }
