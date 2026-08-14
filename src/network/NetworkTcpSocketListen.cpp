#include "NetworkTcpSocket.h"

#include "NetworkTcpSocketAsio.h"

#include <asio/post.hpp>

#include <algorithm>
#include <memory>
#include <string>
#include <system_error>
#include <utility>

namespace ve::network
{
	namespace
	{
		struct AcceptOperationState
		{
			bool active = true;
			std::error_code error;
		};
	}

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
		listening_socket.impl_->open = true;
		return std::optional<TcpSocket>{ std::move(listening_socket) };
	}

	std::optional<TcpSocket> TcpSocket::Accept() const
	{
		return Accept(std::stop_token{});
	}

	std::optional<TcpSocket> TcpSocket::Accept(std::stop_token stop_token) const
	{
		if (!impl_ || !impl_->acceptor.is_open()) return std::nullopt;
		TcpSocket accepted_socket;
		auto state = std::make_shared<AcceptOperationState>();
		impl_->io_context.restart();
		impl_->acceptor.async_accept(accepted_socket.impl_->socket,
			[state](const std::error_code& error)
			{
				state->error = error;
				state->active = false;
			});
		std::stop_callback cancellation(stop_token, [implementation = impl_.get(), state]() noexcept
		{
			try
			{
				asio::post(implementation->io_context, [implementation, state]() noexcept
				{
					if (!state->active) return;
					std::error_code ignored_error;
					implementation->acceptor.cancel(ignored_error);
				});
			}
			catch (...) {}
		});
		impl_->io_context.run();
		if (state->error) return std::nullopt;
		std::error_code error;
		accepted_socket.impl_->socket.non_blocking(true, error);
		if (!error) accepted_socket.impl_->open = true;
		return error ? std::nullopt :
			std::optional<TcpSocket>{ std::move(accepted_socket) };
	}
}
