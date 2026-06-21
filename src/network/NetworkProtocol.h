#pragma once

#include "NetworkTypes.h"

#include <cstdint>
#include <optional>
#include <span>

namespace ve::network
{
	inline constexpr std::uint32_t ProtocolMagic = 0x56454E54;
	inline constexpr std::uint16_t ProtocolVersion = 1;
	inline constexpr std::uint32_t PacketHeaderByteCount = 12;

	struct PacketHeader
	{
		std::uint32_t magic;
		std::uint16_t version;
		NetworkMessageType messageType;
		std::uint32_t payloadByteCount;
	};

	/**
	 * Builds a framed packet from a typed payload.
	 *
	 * @param messageType Protocol message type carried by the packet.
	 * @param payloadBytes Serialized payload bytes.
	 * @return Header and payload bytes ready to send on a TCP stream.
	 */
	ByteBuffer BuildPacket(NetworkMessageType messageType, std::span<const std::byte> payloadBytes);

	/**
	 * Parses and validates a framed packet.
	 *
	 * @param packetBytes Header and payload bytes received from a TCP stream.
	 * @return Parsed message, or empty when magic/version/length are invalid.
	 */
	std::optional<NetworkMessage> TryParsePacket(std::span<const std::byte> packetBytes);
}
