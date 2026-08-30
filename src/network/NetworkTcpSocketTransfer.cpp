#include "NetworkTcpSocket.h"

#include "NetworkTcpSocketAsio.h"

#include <asio/buffer.hpp>
#include <asio/error.hpp>

#include <chrono>
#include <system_error>
#include <thread>

namespace ve::network
{
	namespace
	{
		bool ShouldRetry(const std::error_code& error) noexcept
		{
			return error == asio::error::would_block || error == asio::error::try_again;
		}

		void WaitForSocket() noexcept
		{
			std::this_thread::sleep_for(std::chrono::milliseconds(2));
		}
	}

	bool TcpSocket::SendBytes(std::span<const std::byte> bytes) const
	{
		return SendBytes(bytes, std::stop_token{});
	}

	bool TcpSocket::SendBytes(
		std::span<const std::byte> bytes, std::stop_token stop_token) const
	{
		return SendBytes(bytes, stop_token, std::chrono::steady_clock::time_point::max());
	}

	bool TcpSocket::SendBytes(
		std::span<const std::byte> bytes,
		std::stop_token stop_token,
		std::chrono::steady_clock::time_point deadline) const
	{
		if (!impl_ || impl_->shutdown_requested || stop_token.stop_requested()) return false;
		std::size_t offset = 0;
		while (offset < bytes.size() && !impl_->shutdown_requested &&
			!stop_token.stop_requested() && std::chrono::steady_clock::now() < deadline)
		{
			std::error_code error;
			const std::size_t transferred = impl_->socket.send(
				asio::buffer(bytes.data() + offset, bytes.size() - offset), 0, error);
			if (!error && transferred > 0) offset += transferred;
			else if (ShouldRetry(error)) WaitForSocket();
			else return false;
		}
		return offset == bytes.size();
	}

	bool TcpSocket::ReceiveBytes(std::span<std::byte> destination) const
	{
		if (!impl_ || impl_->shutdown_requested) return false;
		std::size_t offset = 0;
		while (offset < destination.size() && !impl_->shutdown_requested)
		{
			std::error_code error;
			const std::size_t transferred = impl_->socket.receive(
				asio::buffer(destination.data() + offset, destination.size() - offset), 0, error);
			if (!error && transferred > 0) offset += transferred;
			else if (ShouldRetry(error)) WaitForSocket();
			else return false;
		}
		return offset == destination.size();
	}
}
