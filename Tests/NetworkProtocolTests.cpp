#include <doctest/doctest.h>

#include "NetworkProtocol.h"
#include "NetworkSerialization.h"

#include <cstddef>
#include <vector>

TEST_CASE("network block mutation serialization roundtrips signed coordinates")
{
	const ve::network::BlockMutationPayload mutation{ -17, 64, 33, 4 };
	const ve::network::ByteBuffer bytes = ve::network::SerializeBlockMutation(mutation);

	const std::optional<ve::network::BlockMutationPayload> decoded = ve::network::TryDeserializeBlockMutation(bytes);

	REQUIRE(decoded.has_value());
	CHECK(decoded->blockX == -17);
	CHECK(decoded->blockY == 64);
	CHECK(decoded->blockZ == 33);
	CHECK(decoded->blockId == 4);
}

TEST_CASE("network packet parser rejects truncated and corrupted packets")
{
	const ve::network::ByteBuffer payload = ve::network::SerializeClientHello("Renato");
	const ve::network::ByteBuffer packet = ve::network::BuildPacket(ve::network::NetworkMessageType::ClientHello, payload);

	std::vector<std::byte> truncated_packet(packet.begin(), packet.end() - 1);
	CHECK(!ve::network::TryParsePacket(truncated_packet).has_value());

	std::vector<std::byte> corrupt_packet(packet.begin(), packet.end());
	corrupt_packet[0] = std::byte{ 0 };
	CHECK(!ve::network::TryParsePacket(corrupt_packet).has_value());

	const std::optional<ve::network::NetworkMessage> parsed = ve::network::TryParsePacket(packet);
	REQUIRE(parsed.has_value());
	CHECK(parsed->messageType == ve::network::NetworkMessageType::ClientHello);
	CHECK(ve::network::TryDeserializeClientHello(parsed->payloadBytes) == "Renato");
}
