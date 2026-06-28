#include <doctest/doctest.h>

#include "NetworkProtocol.h"
#include "NetworkSerialization.h"

#include <array>
#include <cstddef>
#include <cstring>
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

TEST_CASE("network packet parser rejects oversized payloads before body allocation")
{
	std::array<std::byte, ve::network::PacketHeaderByteCount> header{};
	const std::uint32_t magic = ve::network::ProtocolMagic;
	const std::uint16_t version = ve::network::ProtocolVersion;
	const ve::network::NetworkMessageType message_type = ve::network::NetworkMessageType::ClientHello;
	const std::uint32_t payload_byte_count = ve::network::MaxPacketPayloadByteCount + 1U;
	std::memcpy(header.data(), &magic, sizeof(magic));
	std::memcpy(header.data() + 4, &version, sizeof(version));
	std::memcpy(header.data() + 6, &message_type, sizeof(message_type));
	std::memcpy(header.data() + 8, &payload_byte_count, sizeof(payload_byte_count));

	CHECK(!ve::network::TryParsePacketHeader(header).has_value());

	const ve::network::ByteBuffer oversized_payload(static_cast<std::size_t>(payload_byte_count), std::byte{ 0 });
	CHECK(ve::network::BuildPacket(ve::network::NetworkMessageType::ClientHello, oversized_payload).empty());
}

TEST_CASE("network client hello enforces bounded player names")
{
	std::string long_name(ve::network::MaxPlayerNameByteCount + 8U, 'x');
	const ve::network::ByteBuffer serialized = ve::network::SerializeClientHello(long_name);
	const std::optional<std::string> decoded = ve::network::TryDeserializeClientHello(serialized);

	REQUIRE(decoded.has_value());
	CHECK(decoded->size() == ve::network::MaxPlayerNameByteCount);

	const std::array<std::byte, sizeof(std::uint16_t)> empty_name_payload{};
	CHECK(!ve::network::TryDeserializeClientHello(empty_name_payload).has_value());
}
