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

TEST_CASE("multiplayer server broadcast reports actual recipient writes")
{
	ve::network::MultiplayerServer server;
	const ve::network::NetworkMessage message{
		ve::network::NetworkMessageType::Ping,
		{}
	};

	CHECK(server.Broadcast(message) == 0U);
	CHECK(server.BroadcastExcept(1U, message) == 0U);
}

