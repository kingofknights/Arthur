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

MulticastReceiver::MulticastReceiver(boost::asio::io_service& ioService_, MarketEventQueueT& queue_)
    : CentralFeed(queue_), _socket(ioService_) {
}

void MulticastReceiver::ReceiverFrom(const boost::system::error_code& errorCode_, size_t size_) {
    if (!errorCode_) {
        ProcessData(_buffer, size_);
        Read();
    }
}

void MulticastReceiver::BindMc(const std::string& address_, int port_) {
    _endpoint = boost::asio::ip::udp::endpoint(boost::asio::ip::udp::v4(), port_);
    _socket.open(boost::asio::ip::udp::v4());
    _socket.set_option(boost::asio::socket_base::reuse_address(true));
#ifndef WIN32
    _socket.set_option(boost::asio::detail::socket_option::boolean<SOL_SOCKET, SO_REUSEPORT>(true));
#endif

    _socket.bind(_endpoint);
}

void MulticastReceiver::Read() {
    _socket.async_receive_from(boost::asio::buffer(_buffer, MAX_LENGTH), _endpoint, [this](const boost::system::error_code& errorCode_, size_t size_) { ReceiverFrom(errorCode_, size_); });
}

void MulticastReceiver::ProcessData(char* data_, size_t size_) { Process(data_, size_); }
