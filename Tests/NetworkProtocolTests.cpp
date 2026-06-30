#include <doctest/doctest.h>

#include "NetworkProtocol.h"
#include "NetworkBlockReplication.h"
#include "NetworkSequenceTracker.h"
#include "NetworkSerialization.h"
#include "World.h"

#include <array>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <optional>
#include <string>
#include <vector>

#include "NetworkPacketFramingTests.inl"
#include "NetworkSerializationTests.inl"

TEST_CASE("network sequence tracker rejects duplicate and stale packets")
{
	ve::network::NetworkSequenceTracker sequenceTracker;

	CHECK(sequenceTracker.LastAcceptedSequenceNumber() == 0U);
	CHECK(!sequenceTracker.TryAccept(0U));
	CHECK(sequenceTracker.TryAccept(2U));
	CHECK(sequenceTracker.LastAcceptedSequenceNumber() == 2U);
	CHECK(!sequenceTracker.TryAccept(2U));
	CHECK(!sequenceTracker.TryAccept(1U));
	CHECK(sequenceTracker.TryAccept(5U));

	sequenceTracker.Reset();
	CHECK(sequenceTracker.LastAcceptedSequenceNumber() == 0U);
	CHECK(sequenceTracker.TryAccept(1U));
}

TEST_CASE("network block mutation applies through shared world edit command")
{
	ve::world::World world(ve::world::CreateInfoForSquareWorld(1));
	world.SpawnEmptyGrid(ve::world::FlatWorldSpawnSettings{ 1 });

	const ve::network::BlockMutationPayload blockMutation{
		1U,
		7U,
		3,
		12,
		4,
		static_cast<std::uint8_t>(ve::blocks::BlockId::Stone)
	};
	const ve::network::NetworkMessage message{
		ve::network::NetworkMessageType::BlockMutation,
		ve::network::SerializeBlockMutation(blockMutation)
	};

	CHECK(ve::network::ApplyNetworkBlockMutation(world, message));
	CHECK(world.GetBlock(3, 12, 4) == ve::blocks::BlockId::Stone);
}
