
#include "EpollSocket.hpp"

#include "Logger.hpp"

#include <fcntl.h>

EpollSocket::EpollSocket() : _epollFd(epoll_create1(0)) {}

auto EpollSocket::PrepareMulticastSocket(int streamId_, const std::string& lanIp_, const std::string& multicastIp_, const uint16_t port_) -> int {
    const int file = socket(AF_INET, SOCK_DGRAM, 0);
    if (file < 0) {
        LOG(WARNING, "Opening MC datagram socket num = {} error", streamId_);
        std::exit(EXIT_FAILURE);
    }
    LOG(INFO, "Opening MC datagram socket num = %d....OK.", streamId_);

    constexpr int reuse = 1;
    if (setsockopt(file, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse)) < 0) {
        LOG(ERROR, "Setting SO_REUSEADDR for MC datagram socket num = {} error!!!", streamId_);
        close(file);
        std::exit(EXIT_FAILURE);
    }
    LOG(INFO, "Setting SO_REUSEADDR on MC socket num = {} ...OK.", streamId_);

    sockaddr_in localSock{};
    std::memset(&localSock, 0, sizeof(localSock));
    localSock.sin_family      = AF_INET;
    localSock.sin_addr.s_addr = inet_addr(multicastIp_.data());
    localSock.sin_port        = __bswap_16(port_);

    if (bind(file, reinterpret_cast<sockaddr*>(&localSock), sizeof(sockaddr)) != 0) {
        LOG(WARNING, "Binding MC datagram socket num = {} error, Address : {} port : {}", streamId_, multicastIp_, port_);
        close(file);
        std::exit(EXIT_FAILURE);
    }

    LOG(INFO, "Binding MC datagram socket num = {}...OK.", streamId_);

    if (not lanIp_.empty()) {
        ip_mreq group{};
        group.imr_multiaddr.s_addr = inet_addr(multicastIp_.data());
        group.imr_interface.s_addr = inet_addr(lanIp_.data());
        if (setsockopt(file, IPPROTO_IP, IP_ADD_MEMBERSHIP, &group, sizeof(group)) < 0) {
            LOG(ERROR, "Adding multicast group for socket num = {} error!!!", streamId_);
            close(file);
            std::exit(EXIT_FAILURE);
        }
    }

    constexpr uint64_t recvBufferSize = 16UL * 1024 * 1024;
    if (setsockopt(file, SOL_SOCKET, SO_RCVBUF, &recvBufferSize, sizeof(recvBufferSize)) < 0) {
        LOG(ERROR, "Unable to set buffer size of socket num = {} error!!!", streamId_);
        close(file);
        std::exit(EXIT_FAILURE);
    }
    LOG(INFO, "Adding multicast group for socket num = {}...OK.", file);

    if (fcntl(file, F_SETFL, O_NONBLOCK) < 0) {
        LOG(ERROR, "Failed to set non blocking option to fd {}", file);
    }

    LOG(INFO, "Setting option to non blocking {}", file);
    return file;
}

auto EpollSocket::Construct(const int streamId_, const std::string& lanIp_, const std::string& multicastIp_, const uint16_t port_) const -> int {
    const int sfdReplayA = PrepareMulticastSocket(streamId_, lanIp_, multicastIp_, port_);

    epoll_event eva{};
    eva.events  = EPOLLIN;
    eva.data.fd = sfdReplayA;

    if (epoll_ctl(_epollFd, EPOLL_CTL_ADD, sfdReplayA, &eva) == -1) {
        perror("epoll_ctl");
        std::exit(EXIT_FAILURE);
    }

    return sfdReplayA;
}

void EpollSocket::BindSocket(const std::stop_token& stopToken_) {
    while (not stopToken_.stop_requested()) {
        const int count = epoll_wait(_epollFd, _events.data(), MaxEvents, 0);
        if (count == -1) {
            continue;
        }

        for (size_t index = 0; index < static_cast<size_t>(count); ++index) {
            if ((_events[index].events & EPOLLERR) != 0U || (_events[index].events & EPOLLHUP) != 0U || (_events[index].events & EPOLLIN) == 0U) {
                LOG(ERROR, "{}", "epoll error");
                close(_events[index].data.fd);
                continue;
            }
            if ((_events[index].events & EPOLLIN) != 0U) {
                if (const ssize_t len = recv(_events[index].data.fd, _buffer, 512, 0); len < 0) {
                    close(_events[index].data.fd);
                } else {
                    Process(static_cast<int>(len));
                }
            }
        }
    }
}
auto EpollSocket::GetBuffer() noexcept -> char* {
    return _buffer;
}
