#pragma once

#include "NetworkProtocol.h"
#include "NetworkTcpSocket.h"

#include <stop_token>

namespace ve::network
{
	/**
	 * Sends a complete framed protocol message over a socket.
	 *
	 * @param socket Connected socket that receives the packet bytes.
	 * @param message Typed message and serialized payload.
	 * Writes use a bounded protocol deadline. A failed transfer shuts down the
	 * stream because a partially written frame cannot be reused safely.
	 *
	 * @return True when all bytes were written before cancellation or timeout.
	 */
	bool SendNetworkMessage(const TcpSocket& socket, const NetworkMessage& message);
	bool SendNetworkMessage(
		const TcpSocket& socket, const NetworkMessage& message, std::stop_token stop_token);

	/**
	 * Receives one complete framed protocol message from a socket.
	 *
	 * @param socket Connected socket that provides packet bytes.
	 * @return Parsed message, or empty when the stream closes or the packet is invalid.
	 */
	std::optional<NetworkMessage> ReceiveNetworkMessage(const TcpSocket& socket);
}
