#pragma once

#include "NetworkTypes.h"

#include <cstdint>
#include <optional>
#include <span>

namespace ve::network
{
	struct TcpListenSettings
	{
		/// Endpoint where the socket should listen.
		NetworkEndpoint endpoint;
		/// Maximum number of pending connections accepted by the OS backlog.
		int pendingConnectionBacklog = 8;
	};

	class TcpSocket
	{
	public:
		/**
		 * Creates an empty socket wrapper.
		 */
		TcpSocket() noexcept;

		/**
		 * Wraps an existing platform socket handle.
		 *
		 * @param nativeSocketHandle Platform socket handle stored as an integer.
		 */
		explicit TcpSocket(std::uintptr_t nativeSocketHandle) noexcept;

		/**
		 * Closes the wrapped socket.
		 */
		~TcpSocket();

		TcpSocket(const TcpSocket&) = delete;
		TcpSocket& operator=(const TcpSocket&) = delete;
		TcpSocket(TcpSocket&& other) noexcept;
		TcpSocket& operator=(TcpSocket&& other) noexcept;

		/**
		 * Connects to a remote TCP endpoint.
		 *
		 * @param remoteEndpoint Host and port to connect to.
		 * @return Connected socket, or empty when connection fails.
		 */
		static std::optional<TcpSocket> Connect(const NetworkEndpoint& remoteEndpoint);

		/**
		 * Opens a TCP listening socket.
		 *
		 * @param listenSettings Local endpoint and backlog settings.
		 * @return Listening socket, or empty when bind/listen fails.
		 */
		static std::optional<TcpSocket> Listen(const TcpListenSettings& listenSettings);

		/**
		 * Blocks until one client connects.
		 *
		 * @return Connected client socket, or empty when accept fails.
		 */
		std::optional<TcpSocket> Accept() const;

		/**
		 * Sends every byte in a buffer.
		 *
		 * @param bytes Bytes to send on the TCP stream.
		 * @return True when all bytes were written.
		 */
		bool SendBytes(std::span<const std::byte> bytes) const;

		/**
		 * Receives exactly the requested byte count.
		 *
		 * @param destinationBytes Destination buffer to fill.
		 * @return True when the full buffer was filled.
		 */
		bool ReceiveBytes(std::span<std::byte> destinationBytes) const;

		/**
		 * Closes the socket if it is open.
		 */
		void Close() noexcept;

		/**
		 * Reports whether this wrapper owns an open socket.
		 *
		 * @return True when the socket handle is valid.
		 */
		bool IsOpen() const noexcept;

	private:
		std::uintptr_t _nativeSocketHandle;
	};
}
