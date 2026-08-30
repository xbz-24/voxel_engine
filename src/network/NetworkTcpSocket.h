#pragma once

#include "NetworkTypes.h"

#include <chrono>
#include <memory>
#include <optional>
#include <span>
#include <stop_token>

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

		/** @return Connected peer socket, or empty when accept fails or is stopped. */
		[[nodiscard]] std::optional<TcpSocket> Accept(std::stop_token stop_token) const;

		/** @return True when Asio wrote every byte. */
		[[nodiscard]] bool SendBytes(std::span<const std::byte> bytes) const;
		[[nodiscard]] bool SendBytes(
			std::span<const std::byte> bytes, std::stop_token stop_token) const;

		/** @return True when every byte was written before stop or deadline. */
		[[nodiscard]] bool SendBytes(
			std::span<const std::byte> bytes,
			std::stop_token stop_token,
			std::chrono::steady_clock::time_point deadline) const;

		/** @return True when Asio read exactly the destination size. */
		[[nodiscard]] bool ReceiveBytes(std::span<std::byte> destination) const;

		/** Interrupts synchronous stream transfers without closing the native socket. */
		void Shutdown() const noexcept;

		/** Closes the socket or acceptor after outstanding operations have stopped. */
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
