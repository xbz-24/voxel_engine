#pragma once

#include "NetworkTypes.h"

#include <cstdint>
#include <optional>
#include <span>

namespace ve::network
{
	inline constexpr std::uint32_t ProtocolMagic = 0x56454E54;
	inline constexpr std::uint16_t ProtocolVersion = 2;
	inline constexpr std::uint32_t PacketHeaderByteCount = 20;
	inline constexpr std::uint32_t MaxPacketPayloadByteCount = 64U * 1024U;
	inline constexpr std::uint32_t ProtocolCapabilityPlayerSnapshots = 1U << 0U;
	inline constexpr std::uint32_t ProtocolCapabilityBlockMutations = 1U << 1U;
	inline constexpr std::uint32_t SupportedProtocolCapabilityFlags =
		ProtocolCapabilityPlayerSnapshots | ProtocolCapabilityBlockMutations;

	struct PacketHeader
	{
		std::uint32_t magic;
		std::uint16_t version;
		NetworkMessageType messageType;
		std::uint32_t sequenceNumber;
		std::uint32_t payloadByteCount;
		std::uint32_t payloadChecksum;
	};

	/**
	 * Builds a framed packet from a typed payload.
	 *
	 * @param messageType Protocol message type carried by the packet.
	 * @param payloadBytes Serialized payload bytes.
	 * @param sequenceNumber Optional sender sequence number used for diagnostics and future ordering.
	 * @return Header and payload bytes ready to send on a TCP stream.
	 */
	ByteBuffer BuildPacket(
		NetworkMessageType messageType,
		std::span<const std::byte> payloadBytes,
		std::uint32_t sequenceNumber = 0);

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
