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

#include <future>

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
		ve::blocks::ToBlockByte(ve::blocks::BlockId::Stone)
	};
	const ve::network::NetworkMessage message{
		ve::network::NetworkMessageType::BlockMutation,
		ve::network::SerializeBlockMutation(blockMutation)
	};

	CHECK(ve::network::ApplyNetworkBlockMutation(world, message));
	CHECK(world.GetBlock(3, 12, 4) == ve::blocks::BlockId::Stone);
}

