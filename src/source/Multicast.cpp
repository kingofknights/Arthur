//
// Created by VIKLOD on 26-02-2023.
//

#include "Multicast.hpp"

#include "Arthur_Fwd.hpp"
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
        Process(size_);
        Read();
    } else {
        LOG(WARNING, "ReceiverFrom {}", errorCode_.message());
    }
}

void MulticastReceiver::BindMc(const std::string& address_, int port_, const std::string& multicast_) {
    ErrorCodeT error;
    _endpoint = boost::asio::ip::udp::endpoint(boost::asio::ip::address::from_string(address_), port_);
    error     = _socket.open(_endpoint.protocol(), error);
    LOG(INFO, "Multicast : open {}", error.message());
    error = _socket.set_option(boost::asio::socket_base::reuse_address(true), error);
    _socket.bind(_endpoint);
    LOG(INFO, "Multicast : reuse_address {}", error.message());
    error = _socket.set_option(boost::asio::ip::multicast::join_group(boost::asio::ip::address::from_string(multicast_)), error);
    LOG(INFO, "Multicast : reuse_address {}", error.message());
}

void MulticastReceiver::Read() {
    _socket.async_receive_from(boost::asio::buffer(_buffer, 512), _endpoint, [this](const boost::system::error_code& errorCode_, size_t size_) { ReceiverFrom(errorCode_, size_); });
}
