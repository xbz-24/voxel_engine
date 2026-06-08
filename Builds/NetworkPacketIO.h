#pragma once

#include "NetworkProtocol.h"
#include "NetworkTcpSocket.h"

namespace ve::network
{
	/**
	 * Sends a complete framed protocol message over a socket.
	 *
	 * @param socket Connected socket that receives the packet bytes.
	 * @param message Typed message and serialized payload.
	 * @return True when all bytes were written.
	 */
	bool SendNetworkMessage(const TcpSocket& socket, const NetworkMessage& message);

	/**
	 * Receives one complete framed protocol message from a socket.
	 *
	 * @param socket Connected socket that provides packet bytes.
	 * @return Parsed message, or empty when the stream closes or the packet is invalid.
	 */
	std::optional<NetworkMessage> ReceiveNetworkMessage(const TcpSocket& socket);
}
