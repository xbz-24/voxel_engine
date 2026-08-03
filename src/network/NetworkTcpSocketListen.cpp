#include "NetworkTcpSocket.h"

#include "NetworkTcpSocketAsio.h"

#include <algorithm>
#include <string>
#include <system_error>
#include <utility>

namespace ve::network
{
	std::optional<TcpSocket> TcpSocket::Listen(const TcpListenSettings& settings)
	{
		TcpSocket listening_socket;
		std::error_code error;
		asio::ip::tcp::endpoint endpoint(asio::ip::tcp::v4(), settings.endpoint.port);
		if (!settings.endpoint.hostName.empty())
		{
			asio::ip::tcp::resolver resolver(listening_socket.impl_->io_context);
			const auto endpoints = resolver.resolve(
				settings.endpoint.hostName,
				std::to_string(settings.endpoint.port),
				error);
			if (error || endpoints.empty()) return std::nullopt;
			endpoint = endpoints.begin()->endpoint();
		}

		auto& acceptor = listening_socket.impl_->acceptor;
		acceptor.open(endpoint.protocol(), error);
		if (error) return std::nullopt;
		acceptor.set_option(asio::socket_base::reuse_address(true), error);
		if (error) return std::nullopt;
		acceptor.bind(endpoint, error);
		if (error) return std::nullopt;
		acceptor.listen(std::max(settings.pendingConnectionBacklog, 1), error);
		if (error) return std::nullopt;
		return std::optional<TcpSocket>{ std::move(listening_socket) };
	}

	std::optional<TcpSocket> TcpSocket::Accept() const
	{
		if (!impl_ || !impl_->acceptor.is_open()) return std::nullopt;
		TcpSocket accepted_socket;
		std::error_code error;
		impl_->acceptor.accept(accepted_socket.impl_->socket, error);
		if (error) return std::nullopt;
		return std::optional<TcpSocket>{ std::move(accepted_socket) };
	}
}
