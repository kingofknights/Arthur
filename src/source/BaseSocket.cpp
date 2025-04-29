#include "include/BaseSocket.hpp"

#include "Lancelot/Logger.hpp"
#include "Lancelot/Structure.hpp"

TBaseSocket::TBaseSocket(ExecutorT& executor_) : _socket(executor_), _timer(executor_), _buffer{}, _header(reinterpret_cast<Lancelot::Header*>(_buffer.data())) {
}

void TBaseSocket::MakeConnection(const std::string& address_, uint16_t port_) {
    _address = address_;
    _port    = port_;
    boost::asio::ip::tcp::resolver        resolver(_socket.get_executor());
    boost::asio::ip::tcp::resolver::query query(address_, std::to_string(port_));

    auto endpoint = resolver.resolve(query);

    Connect(endpoint);
}

void TBaseSocket::Connect(const EndpointIteratorT& endpoint_) {
    _socket.async_connect(*endpoint_, [this, endpoint_](auto&& errorCode_) { InternalConnectHandler(errorCode_, endpoint_); });
}

void TBaseSocket::InternalConnectHandler(const ErrorCodeT& error_code_, const EndpointIteratorT& endpoint_) {
    if (not _socket.is_open()) {
        LOG(WARNING, "Socket got closed ! Trying again!! {}", error_code_.message());
        Reconnect(endpoint_);
        _connected = false;
    } else if (error_code_) {
        LOG(WARNING, "Connect error: {}", error_code_.message());
        _socket.close();
        Reconnect(endpoint_);
        _connected = false;
    } else {
        LOG(INFO, "Connected to {}", endpoint_->endpoint().address().to_string());
        _connected = true;
        _socket.set_option(boost::asio::socket_base::keep_alive(true));
        _socket.set_option(boost::asio::socket_base::reuse_address(true));
        _socket.set_option(boost::asio::ip::tcp::no_delay(true));

        ConnectedStatus(_connected);
        Read();
    }
}

void TBaseSocket::WriteAsync(void* buffer_, size_t size_) {
    boost::asio::async_write(_socket, boost::asio::buffer(buffer_, size_),
                             boost::asio::transfer_exactly(size_),
                             [](const ErrorCodeT& errorCode_, size_t size_) {
                                 (void)errorCode_;
                                 (void)size_;
                             });
}

void TBaseSocket::WriteSync(void* buffer_, size_t size_) {
    boost::asio::write(_socket, boost::asio::buffer(buffer_, size_), boost::asio::transfer_exactly(size_), _errorCode);
}

void TBaseSocket::Read() {
    boost::asio::async_read(_socket, boost::asio::buffer(_buffer.data(), sizeof(Lancelot::Header)), boost::asio::transfer_exactly(sizeof(Lancelot::Header)),
                            [this](const ErrorCodeT& errorCode_, size_t size_) {
                                ReadHandlerBody(errorCode_, size_);
                            });
}

void TBaseSocket::ReadHandlerBody(const ErrorCodeT& errorCode_, size_t size_) {
    if (not errorCode_) {
        boost::asio::async_read(_socket, boost::asio::buffer(_buffer.data() + sizeof(Lancelot::Header), static_cast<size_t>(_header->_length)),
                                boost::asio::transfer_exactly(static_cast<size_t>(_header->_length)),
                                [this](const boost::system::error_code& error_, size_t size_) {
                                    if (not error_) {
                                        Process(_buffer.data(), size_);
                                        Read();
                                    } else {
                                        _connected = false;
                                    }
                                });
    } else {
        LOG(WARNING, "Read error: {} {}", errorCode_.message(), size_);
        _errorCode = _socket.close(_errorCode);
        _connected = false;

        ConnectedStatus(_connected);

        MakeConnection(_address, _port);
    }
}

void TBaseSocket::Reconnect(EndpointIteratorT endpoint_) noexcept {
    _timer.expires_from_now(boost::posix_time::seconds(5));
    _timer.async_wait([endpoint = std::move(endpoint_), this](const ErrorCodeT& errorCode_) {
        if (not errorCode_) {
            Connect(endpoint);
        }
    });
}
auto TBaseSocket::IsConnected() const noexcept -> bool {
    return _connected;
}
