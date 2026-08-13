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

