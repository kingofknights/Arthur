#ifndef ARTHUR_INCLUDE_BASE_SOCKET_HPP
#define ARTHUR_INCLUDE_BASE_SOCKET_HPP
#pragma once

#include <boost/asio.hpp>
#include <memory>

class TBaseSocket {
public:
	explicit TBaseSocket(boost::asio::io_context &ioContext_);

	virtual ~TBaseSocket() = default;

	void makeConnection(const std::string &IpAddress, const std::string &Port);
	void Write_Async(const char *buffer, size_t size_);
	void Write_Sync(char *buffer, size_t size_);

protected:
	virtual void process(const char *buffer_, size_t size_) = 0;

	void Read();
	void ReadHandlerBody(const boost::system::error_code &error_code_, size_t size_);
	void WriteHandler(const boost::system::error_code &errorCode_, size_t size_){};

private:
	void Connect(const boost::asio::ip::tcp::resolver::iterator &iterator_);
	void internalConnectHandler(const boost::system::error_code &error_code_, boost::asio::ip::tcp::resolver::iterator iterator_);

	char _buffer[1024]{};

	std::string _ipAddress;
	std::string _port;

	boost::system::error_code						 _errorCode;
	std::shared_ptr<boost::asio::io_context::strand> _strand;
	std::shared_ptr<boost::asio::ip::tcp::socket>	 _socket;
};

#endif	// ARTHUR_INCLUDE_BASE_SOCKET_HPP
