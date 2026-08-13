#include <doctest/doctest.h>

#include "NetworkProtocol.h"
#include "NetworkBlockReplication.h"
#include "MultiplayerServer.h"
#include "NetworkSequenceTracker.h"
#include "NetworkSession.h"
#include "NetworkSerialization.h"
#include "World.h"
#include "TestTypeHelpers.h"
#include "NetworkPacketTestSupport.h"

#include <array>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <optional>
#include <string>
#include <vector>

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
