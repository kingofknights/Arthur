//
// Created by VIKLOD on 26-02-2023.
//

#include "../include/Multicast.hpp"

MulticastReceiver::MulticastReceiver(boost::asio::io_service& ioService_) : _socket(ioService_) {}

void MulticastReceiver::receiverFrom(const boost::system::error_code& errorCode_, size_t size_) {
    if (!errorCode_) {
        processData(_buffer, size_);
        read();
    }
}

void MulticastReceiver::bindMC(const std::string& lan_, const std::string& address_, int port_) {
    boost::asio::ip::udp::endpoint listen_endpoint(boost::asio::ip::address::from_string(lan_), port_);
    _socket.open(listen_endpoint.protocol());
    _socket.set_option(boost::asio::ip::udp::socket::reuse_address(true));
    _socket.bind(listen_endpoint);
    _socket.set_option(boost::asio::ip::multicast::join_group(boost::asio::ip::address::from_string(address_)));
}

void MulticastReceiver::read() {
    _socket.async_receive_from(boost::asio::buffer(_buffer, MAX_LENGTH), _endpoint, [this](const boost::system::error_code& errorCode_, size_t size_) { receiverFrom(errorCode_, size_); });
}

void MulticastReceiver::processData(char* data_, size_t size_) { Process(data_, size_); }
