#pragma once

#include <cstddef>
#include <cstdint>
#include <string>
#include <vector>

namespace ve::network
{
	/**
	 * Byte container used by protocol serialization and packet framing.
	 */
	using ByteBuffer = std::vector<std::byte>;

	/**
	 * Identifies a remote or local TCP endpoint.
	 */
	struct NetworkEndpoint
	{
		/// DNS name or IPv4 address. Empty host means all interfaces for listening sockets.
		std::string hostName;
		/// TCP port in host byte order.
		std::uint16_t port;
	};

	/**
	 * Message kinds supported by the voxel engine protocol.
	 */
	enum class NetworkMessageType : std::uint16_t
	{
		ClientHello = 1,
		ServerWelcome = 2,
		PlayerSnapshot = 3,
		BlockMutation = 4,
		Ping = 5,
		Pong = 6,
		Disconnect = 7
	};

	/**
	 * Complete protocol message after packet framing has been removed.
	 */
	struct NetworkMessage
	{
		/// Message kind used by the receiver to dispatch payload decoding.
		NetworkMessageType messageType;
		/// Serialized message body bytes.
		ByteBuffer payloadBytes;
	};
}
