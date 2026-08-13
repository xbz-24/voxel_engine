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

TEST_CASE("network packet parser rejects oversized and unknown payload headers")
{
	const std::array<std::byte, ve::network::PacketHeaderByteCount> oversizedHeaderBytes = BuildPacketHeaderForTest(
		ve::network::NetworkMessageType::ClientHello,
		ve::network::MaxPacketPayloadByteCount + 1U);
	CHECK(!ve::network::TryParsePacketHeader(oversizedHeaderBytes).has_value());

	const auto unknownMessageType = ve::tests::InvalidEnumValue<ve::network::NetworkMessageType>(999);
	const std::array<std::byte, ve::network::PacketHeaderByteCount> unknownHeaderBytes = BuildPacketHeaderForTest(
		unknownMessageType,
		0);
	CHECK(!ve::network::TryParsePacketHeader(unknownHeaderBytes).has_value());

	const ve::network::ByteBuffer oversizedPayloadBytes(
		ve::core::ToIndex(ve::network::MaxPacketPayloadByteCount) + 1U,
		std::byte{ 0 });
	CHECK(ve::network::BuildPacket(ve::network::NetworkMessageType::ClientHello, oversizedPayloadBytes).empty());
	CHECK(ve::network::BuildPacket(unknownMessageType, {}).empty());
}
