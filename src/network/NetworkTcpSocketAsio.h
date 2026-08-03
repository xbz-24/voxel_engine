#pragma once

#include "NetworkTcpSocket.h"

#include <asio/io_context.hpp>
#include <asio/ip/tcp.hpp>

namespace ve::network
{
	class TcpSocket::Impl
	{
	public:
		Impl() : socket(io_context), acceptor(io_context) {}

		asio::io_context io_context;
		asio::ip::tcp::socket socket;
		asio::ip::tcp::acceptor acceptor;
	};
}
