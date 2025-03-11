//
// Created by VIKLOD on 26-02-2023.
//

#pragma once

#include <boost/asio.hpp>

#include "../DataFeed/CentralFeed.hpp"

class MulticastReceiver : protected CentralFeed {
  public:
    explicit MulticastReceiver(boost::asio::io_service& ioService_);
    void bindMC(const std::string& address_, int port_);
    void read();

  protected:
    void processData(char* data_, size_t size_);
    void receiverFrom(const boost::system::error_code& errorCode_, size_t size_);

  private:
    boost::asio::ip::udp::socket   _socket;
    boost::asio::ip::udp::endpoint _endpoint;
    enum {
        MAX_LENGTH = 1024
    };
    char _buffer[MAX_LENGTH]{};
};
