#pragma once

#include "NetworkTypes.h"

#include <cstdint>
#include <optional>
#include <span>

namespace ve::network
{
	inline constexpr std::uint32_t ProtocolMagic = 0x56454E54;
	inline constexpr std::uint16_t ProtocolVersion = 1;
	// TODO: Add protocol capability negotiation before changing payload shapes in multiplayer saves.
	inline constexpr std::uint32_t PacketHeaderByteCount = 12;
	inline constexpr std::uint32_t MaxPacketPayloadByteCount = 64U * 1024U;

	struct PacketHeader
	{
		// TODO: Add sequence number and checksum fields for better disconnect/error diagnostics.
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
	 * Parses and validates one packet header.
	 *
	 * @param headerBytes Exactly one packet header.
	 * @return Parsed header, or empty when magic/version/length are invalid.
	 */
	std::optional<PacketHeader> TryParsePacketHeader(std::span<const std::byte> headerBytes);

	/**
	 * Parses and validates a framed packet.
	 *
	 * @param packetBytes Header and payload bytes received from a TCP stream.
	 * @return Parsed message, or empty when magic/version/length are invalid.
	 */
	std::optional<NetworkMessage> TryParsePacket(std::span<const std::byte> packetBytes);
}
