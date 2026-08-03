#pragma once

#include "NetworkTypes.h"

#include <memory>
#include <optional>
#include <span>

namespace ve::network
{
	struct TcpListenSettings
	{
		NetworkEndpoint endpoint;
		int pendingConnectionBacklog = 8;
	};

	/** Move-only synchronous TCP adapter backed by standalone Asio. */
	class TcpSocket
	{
	public:
		~TcpSocket();

		TcpSocket(const TcpSocket&) = delete;
		TcpSocket& operator=(const TcpSocket&) = delete;
		TcpSocket(TcpSocket&& other) noexcept;
		TcpSocket& operator=(TcpSocket&& other) noexcept;

		/** @return Connected socket, or empty when resolution or connection fails. */
		[[nodiscard]] static std::optional<TcpSocket> Connect(const NetworkEndpoint& remote_endpoint);

		/** @return Listening socket, or empty when resolution, bind, or listen fails. */
		[[nodiscard]] static std::optional<TcpSocket> Listen(const TcpListenSettings& settings);

		/** @return Connected peer socket, or empty when accept fails. */
		[[nodiscard]] std::optional<TcpSocket> Accept() const;

		/** @return True when Asio wrote every byte. */
		[[nodiscard]] bool SendBytes(std::span<const std::byte> bytes) const;

		/** @return True when Asio read exactly the destination size. */
		[[nodiscard]] bool ReceiveBytes(std::span<std::byte> destination) const;

		/** Cancels outstanding work and closes the socket or acceptor. */
		void Close() noexcept;

		/** @return True when this object owns an open socket or acceptor. */
		[[nodiscard]] bool IsOpen() const noexcept;

		/** @return Bound local endpoint, or empty when unavailable. */
		[[nodiscard]] std::optional<NetworkEndpoint> LocalEndpoint() const;

	private:
		class Impl;
		TcpSocket();
		std::unique_ptr<Impl> impl_;
	};
}
