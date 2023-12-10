#include "../include/BaseSocket.hpp"

#include "../API/Common.hpp"
#include "../include/Enums.hpp"
#include "../include/Structure.hpp"
#include "../include/Utils.hpp"

extern int  UserID;
extern bool BackendConnected;
TBaseSocket::TBaseSocket(boost::asio::io_context& ioContext_) {
    this->_socket = std::make_shared<boost::asio::ip::tcp::socket>(ioContext_);
    this->_strand = std::make_shared<boost::asio::io_service::strand>(ioContext_);
}

void TBaseSocket::makeConnection(const std::string& IpAddress_, const std::string& Port_) {
    _ipAddress = IpAddress_;
    _port      = Port_;

    boost::asio::ip::tcp::resolver        resolver(_socket->get_executor());
    boost::asio::ip::tcp::resolver::query query(_ipAddress, _port);
    auto                                  endpoint_ = resolver.resolve(query);

    Connect(endpoint_);
}

void TBaseSocket::Connect(const boost::asio::ip::tcp::resolver::iterator& iterator_) {
    this->_socket->async_connect(*iterator_, [this, iterator_](auto&& PH1) { internalConnectHandler(std::forward<decltype(PH1)>(PH1), iterator_); });
}

void TBaseSocket::internalConnectHandler(const boost::system::error_code& error_code_, boost::asio::ip::tcp::resolver::iterator iterator_) {
    if (!this->_socket->is_open()) {
        LOG(WARNING, "Socket got closed ! Trying again!! {}", error_code_.message());
        Connect(iterator_);
        BackendConnected = false;
    } else if (error_code_) {
        LOG(WARNING, "Connect error: {}", error_code_.message());
        this->_socket->close();
        Connect(iterator_);
        BackendConnected = false;
    } else {
        LOG(INFO, "Connected to {}", iterator_->endpoint().address().to_string());
        BackendConnected = true;
        _socket->set_option(boost::asio::socket_base::keep_alive(true));
        _socket->set_option(boost::asio::socket_base::reuse_address(true));
        _socket->set_option(boost::asio::ip::tcp::no_delay(true));
        Utils::ResetPortfolio(StrategyStatus_INACTIVE);

        Lancelot::CommunicationT communication = Lancelot::Encrypt("", UserID, Lancelot::RequestType_LOGIN);
        Write_Sync((char*)&communication, sizeof(Lancelot::CommunicationT));
        Read();
    }
}

void TBaseSocket::Write_Async(const char* buffer, size_t size_) {
    boost::asio::async_write(*this->_socket, boost::asio::buffer(buffer, size_), boost::asio::transfer_exactly(size_), [this](const boost::system::error_code& errorCode_, size_t size_) { WriteHandler(errorCode_, size_); });
}

void TBaseSocket::Write_Sync(char* buffer, size_t size_) { boost::asio::write(*this->_socket, boost::asio::buffer(buffer, size_), boost::asio::transfer_exactly(size_), _errorCode); }

void TBaseSocket::Read() {
    boost::asio::async_read(*this->_socket, boost::asio::buffer(_buffer, sizeof(Lancelot::CommunicationT)), boost::asio::transfer_exactly(sizeof(Lancelot::CommunicationT)), [this](const boost::system::error_code& error_code_, size_t size_) { ReadHandlerBody(error_code_, size_); });
}

void TBaseSocket::ReadHandlerBody(const boost::system::error_code& error_code_, size_t size_) {
    if (!error_code_) {
        process(_buffer, size_);
        Read();
    } else {
        LOG(WARNING, "Read error: {}", error_code_.message());
        _socket->close();
        Utils::ResetPortfolio(StrategyStatus_DISCONNECTED);
        makeConnection(_ipAddress, _port);
    }
}
