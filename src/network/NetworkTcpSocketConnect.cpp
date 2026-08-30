#include "NetworkTcpSocket.h"

#include "NetworkTcpSocketAsio.h"

#include <asio/connect.hpp>

#include <string>
#include <system_error>
#include <utility>

namespace ve::network
{
	std::optional<TcpSocket> TcpSocket::Connect(const NetworkEndpoint& remote_endpoint)
	{
		TcpSocket connected_socket;
		asio::ip::tcp::resolver resolver(connected_socket.impl_->io_context);
		std::error_code error;
		const auto endpoints = resolver.resolve(
			remote_endpoint.hostName,
			std::to_string(remote_endpoint.port),
			error);
		if (error) return std::nullopt;
		asio::connect(connected_socket.impl_->socket, endpoints, error);
		if (error) return std::nullopt;
		connected_socket.impl_->socket.non_blocking(true, error);
		if (error) return std::nullopt;
		connected_socket.impl_->open = true;
		return std::optional<TcpSocket>{ std::move(connected_socket) };
	}
}
