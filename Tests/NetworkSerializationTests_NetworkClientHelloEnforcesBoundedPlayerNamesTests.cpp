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

TEST_CASE("network client hello enforces bounded player names")
{
	std::string oversizedPlayerName(ve::network::MaxPlayerNameByteCount + 8U, 'x');
	const ve::network::ByteBuffer serializedPayloadBytes = ve::network::SerializeClientHello(oversizedPlayerName);
	const std::optional<std::string> decodedPlayerName = ve::network::TryDeserializeClientHello(serializedPayloadBytes);

	REQUIRE(decodedPlayerName.has_value());
	CHECK(decodedPlayerName->size() == ve::network::MaxPlayerNameByteCount);

	const std::array<std::byte, sizeof(std::uint16_t)> emptyNamePayloadBytes{};
	CHECK(!ve::network::TryDeserializeClientHello(emptyNamePayloadBytes).has_value());
}

