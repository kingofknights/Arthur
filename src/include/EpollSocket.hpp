#pragma once

#include <sys/epoll.h>

class EpollSocket {
  public:
    explicit EpollSocket();

    virtual ~EpollSocket() = default;

    [[nodiscard]] auto Construct(int streamId_, const std::string& lanIp_, const std::string& multicastIp_, uint16_t port_) const -> int;

    void BindSocket(const std::stop_token& stopToken_);

    virtual void Process(int size_) = 0;

  protected:
    [[nodiscard]] static auto PrepareMulticastSocket(int streamId_, const std::string& lanIp_, const std::string& multicastIp_, uint16_t port_) -> int;

    [[nodiscard]] auto GetBuffer() noexcept -> char*;

  private:
    static constexpr int MaxEvents  = 1024;
    static constexpr int BufferSize = 999999;

    const int _epollFd;
    char      _buffer[512];

    std::array<epoll_event, MaxEvents> _events{};
};
