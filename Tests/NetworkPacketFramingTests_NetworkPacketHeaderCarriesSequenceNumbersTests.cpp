#include <doctest/doctest.h>

#include "NetworkProtocol.h"
#include "NetworkBlockReplication.h"
#include "MultiplayerServer.h"
#include "NetworkSequenceTracker.h"
#include "NetworkSession.h"
#include "NetworkSerialization.h"
#include "World.h"
#include "TestTypeHelpers.h"

#include <array>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <optional>
#include <string>
#include <vector>

namespace
{
	template <typename Value>
	void WritePacketHeaderField(
		std::array<std::byte, ve::network::PacketHeaderByteCount>& headerBytes,
		std::size_t& headerByteOffset,
		const Value& value)
	{
		std::memcpy(headerBytes.data() + headerByteOffset, &value, sizeof(Value));
		headerByteOffset += sizeof(Value);
	}

	std::array<std::byte, ve::network::PacketHeaderByteCount> BuildPacketHeaderForTest(
		ve::network::NetworkMessageType messageType,
		std::uint32_t payloadByteCount)
	{
		std::array<std::byte, ve::network::PacketHeaderByteCount> headerBytes{};
		std::size_t headerByteOffset = 0;
		const std::uint32_t magic = ve::network::ProtocolMagic;
		const std::uint16_t version = ve::network::ProtocolVersion;
		const std::uint32_t sequenceNumber = 0;
		const std::uint32_t payloadChecksum = 0;
		WritePacketHeaderField(headerBytes, headerByteOffset, magic);
		WritePacketHeaderField(headerBytes, headerByteOffset, version);
		WritePacketHeaderField(headerBytes, headerByteOffset, messageType);
		WritePacketHeaderField(headerBytes, headerByteOffset, sequenceNumber);
		WritePacketHeaderField(headerBytes, headerByteOffset, payloadByteCount);
		WritePacketHeaderField(headerBytes, headerByteOffset, payloadChecksum);
		return headerBytes;
	}
}

TEST_CASE("network packet header carries sequence numbers")
{
	const ve::network::ByteBuffer payload = ve::network::SerializeClientHello("Renato");
	const ve::network::ByteBuffer packet = ve::network::BuildPacket(ve::network::NetworkMessageType::ClientHello, payload, 42);
	const std::span<const std::byte> packetHeaderBytes(packet.data(), ve::network::PacketHeaderByteCount);
	const std::optional<ve::network::PacketHeader> header = ve::network::TryParsePacketHeader(packetHeaderBytes);
	const std::optional<ve::network::NetworkMessage> parsed = ve::network::TryParsePacket(packet);

	REQUIRE(header.has_value());
	REQUIRE(parsed.has_value());
	CHECK(header->sequenceNumber == 42U);
	CHECK(parsed->sequenceNumber == 42U);
	CHECK(header->payloadByteCount == ve::core::ToU32(payload.size()));
}

