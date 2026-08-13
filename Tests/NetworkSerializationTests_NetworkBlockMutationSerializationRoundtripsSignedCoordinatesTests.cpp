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

TEST_CASE("network block mutation serialization roundtrips signed coordinates")
{
	const ve::network::BlockMutationPayload blockMutation{ 99U, 7U, -17, 64, 33, std::uint8_t{ 4 } };
	const ve::network::ByteBuffer serializedPayloadBytes = ve::network::SerializeBlockMutation(blockMutation);
	const std::size_t expectedWireSize = (2U * sizeof(std::uint32_t)) + (3U * sizeof(std::int32_t)) + sizeof(std::uint8_t);

	CHECK(serializedPayloadBytes.size() == expectedWireSize);
	const std::optional<ve::network::BlockMutationPayload> decodedBlockMutation =
		ve::network::TryDeserializeBlockMutation(serializedPayloadBytes);

	REQUIRE(decodedBlockMutation.has_value());
	CHECK(decodedBlockMutation->mutationId == 99U);
	CHECK(decodedBlockMutation->authorPlayerId == 7U);
	CHECK(decodedBlockMutation->blockX == -17);
	CHECK(decodedBlockMutation->blockY == 64);
	CHECK(decodedBlockMutation->blockZ == 33);
	CHECK(decodedBlockMutation->blockId == 4);
}

