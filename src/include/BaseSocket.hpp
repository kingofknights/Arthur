#pragma once

#include "Arthur_Fwd.hpp"

class TBaseSocket {
    using EndpointT         = boost::asio::ip::tcp::endpoint;
    using SocketT           = boost::asio::ip::tcp::socket;
    using BufferT           = std::array<char, 1024>;
    using EndpointIteratorT = boost::asio::ip::tcp::resolver::iterator;

  public:
    explicit TBaseSocket(ExecutorT& executor_);

    virtual ~TBaseSocket() = default;

    void MakeConnection(const std::string& address_, uint16_t port_);

    void WriteAsync(void* buffer_, size_t size_);

    void WriteSync(void* buffer_, size_t size_);

    [[nodiscard]] auto IsConnected() const noexcept -> bool;

  protected:
    virtual void Process(const char* buffer_, size_t size_) = 0;

    void Read();

    void ReadHandlerBody(const ErrorCodeT& errorCode_, size_t size_);

    void Reconnect(EndpointIteratorT endpoint_) noexcept;

  private:
    void Connect(const EndpointIteratorT& endpoint_);

    void InternalConnectHandler(const ErrorCodeT& error_code_, const EndpointIteratorT& endpoint_);

    bool _connected = false;

    ExecutorStrandT   _strand;
    SocketT           _socket;
    TimerT            _timer;
    ErrorCodeT        _errorCode;
    BufferT           _buffer;
    Lancelot::Header* _header;
};
