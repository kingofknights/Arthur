//
// Created by VIKLOD on 26-02-2023.
//

#pragma once

#include "../DataFeed/CentralFeed.hpp"
#include "include/Structure.hpp"

#include <boost/asio.hpp>

class MulticastReceiver : protected CentralFeed {
  public:
    explicit MulticastReceiver(boost::asio::io_service& ioService_, MarketEventQueueT& queue_);

    void BindMc(const std::string& address_, int port_);

    void Read();

  protected:
    void ProcessData(char* data_, size_t size_);

    void ReceiverFrom(const boost::system::error_code& errorCode_, size_t size_);

  private:
    boost::asio::ip::udp::socket   _socket;
    boost::asio::ip::udp::endpoint _endpoint;
    enum {
        MAX_LENGTH = 1024
    };
    char _buffer[MAX_LENGTH]{};
};
