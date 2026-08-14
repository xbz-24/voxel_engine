#include "NetworkTcpSocket.h"

#include "NetworkTcpSocketAsio.h"

#include <string>
#include <system_error>
#include <utility>

namespace ve::network
{
	TcpSocket::TcpSocket() : impl_(std::make_unique<Impl>()) {}
	TcpSocket::~TcpSocket() { Close(); }
	TcpSocket::TcpSocket(TcpSocket&& other) noexcept = default;
	TcpSocket& TcpSocket::operator=(TcpSocket&& other) noexcept = default;

	void TcpSocket::Shutdown() const noexcept
	{
		if (!impl_) return;
		impl_->shutdown_requested = true;
		std::error_code ignored_error;
		impl_->socket.shutdown(asio::ip::tcp::socket::shutdown_both, ignored_error);
	}

	void TcpSocket::Close() noexcept
	{
		if (!impl_) return;
		impl_->open = false;
		impl_->shutdown_requested = true;
		std::error_code ignored_error;
		if (impl_->acceptor.is_open())
		{
			impl_->acceptor.cancel(ignored_error);
			impl_->acceptor.close(ignored_error);
		}
		if (impl_->socket.is_open())
		{
			impl_->socket.cancel(ignored_error);
			impl_->socket.shutdown(asio::ip::tcp::socket::shutdown_both, ignored_error);
			impl_->socket.close(ignored_error);
		}
	}

	bool TcpSocket::IsOpen() const noexcept
	{
		return impl_ && impl_->open;
	}

	std::optional<NetworkEndpoint> TcpSocket::LocalEndpoint() const
	{
		if (!impl_) return std::nullopt;
		std::error_code error;
		const asio::ip::tcp::endpoint endpoint = impl_->acceptor.is_open()
			? impl_->acceptor.local_endpoint(error)
			: impl_->socket.local_endpoint(error);
		if (error) return std::nullopt;
		const std::string host_name = endpoint.address().to_string(error);
		return error ? std::nullopt : std::optional<NetworkEndpoint>{ NetworkEndpoint{ host_name, endpoint.port() } };
	}
}
