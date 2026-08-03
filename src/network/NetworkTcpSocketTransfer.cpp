#include "NetworkTcpSocket.h"

#include "NetworkTcpSocketAsio.h"

#include <asio/buffer.hpp>
#include <asio/read.hpp>
#include <asio/write.hpp>

#include <system_error>

namespace ve::network
{
	bool TcpSocket::SendBytes(std::span<const std::byte> bytes) const
	{
		if (!impl_ || !impl_->socket.is_open()) return false;
		std::error_code error;
		const std::size_t transferred = asio::write(
			impl_->socket,
			asio::buffer(bytes.data(), bytes.size()),
			error);
		return !error && transferred == bytes.size();
	}

	bool TcpSocket::ReceiveBytes(std::span<std::byte> destination) const
	{
		if (!impl_ || !impl_->socket.is_open()) return false;
		std::error_code error;
		const std::size_t transferred = asio::read(
			impl_->socket,
			asio::buffer(destination.data(), destination.size()),
			error);
		return !error && transferred == destination.size();
	}
}
