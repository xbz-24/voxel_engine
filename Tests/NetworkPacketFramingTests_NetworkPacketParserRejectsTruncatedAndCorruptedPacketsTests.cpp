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

TEST_CASE("network packet parser rejects truncated and corrupted packets")
{
	const ve::network::ByteBuffer payload = ve::network::SerializeClientHello("Renato");
	const ve::network::ByteBuffer packet = ve::network::BuildPacket(ve::network::NetworkMessageType::ClientHello, payload);

	std::vector<std::byte> truncatedPacketBytes(packet.begin(), packet.end() - 1);
	CHECK(!ve::network::TryParsePacket(truncatedPacketBytes).has_value());

	std::vector<std::byte> corruptHeaderPacketBytes(packet.begin(), packet.end());
	corruptHeaderPacketBytes[0] = std::byte{ 0 };
	CHECK(!ve::network::TryParsePacket(corruptHeaderPacketBytes).has_value());

	std::vector<std::byte> corruptPayloadPacketBytes(packet.begin(), packet.end());
	corruptPayloadPacketBytes.back() = std::byte{ 0x7f };
	CHECK(!ve::network::TryParsePacket(corruptPayloadPacketBytes).has_value());

	const std::optional<ve::network::NetworkMessage> parsed = ve::network::TryParsePacket(packet);
	REQUIRE(parsed.has_value());
	CHECK(parsed->messageType == ve::network::NetworkMessageType::ClientHello);
	CHECK(parsed->sequenceNumber == 0U);
	CHECK(ve::network::TryDeserializeClientHello(parsed->payloadBytes) == "Renato");
}

