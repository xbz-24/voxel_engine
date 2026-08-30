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

TEST_CASE("network player snapshot serialization uses explicit wire fields")
{
	const ve::network::PlayerSnapshotPayload playerSnapshot{
		7U,
		120U,
		1.0f,
		2.0f,
		3.0f,
		0.5f,
		-0.25f,
		1.5f,
		45.0f,
		-10.0f
	};
	ve::network::ByteBuffer serializedPayloadBytes = ve::network::SerializePlayerSnapshot(playerSnapshot);
	const std::size_t expectedWireSize = (2U * sizeof(std::uint32_t)) + (8U * sizeof(float));

	CHECK(serializedPayloadBytes.size() == expectedWireSize);
	const std::optional<ve::network::PlayerSnapshotPayload> decodedPlayerSnapshot =
		ve::network::TryDeserializePlayerSnapshot(serializedPayloadBytes);

	REQUIRE(decodedPlayerSnapshot.has_value());
	CHECK(decodedPlayerSnapshot->playerId == 7U);
	CHECK(decodedPlayerSnapshot->simulationTickId == 120U);
	CHECK(decodedPlayerSnapshot->positionX == doctest::Approx(1.0f));
	CHECK(decodedPlayerSnapshot->positionY == doctest::Approx(2.0f));
	CHECK(decodedPlayerSnapshot->positionZ == doctest::Approx(3.0f));
	CHECK(decodedPlayerSnapshot->velocityX == doctest::Approx(0.5f));
	CHECK(decodedPlayerSnapshot->velocityY == doctest::Approx(-0.25f));
	CHECK(decodedPlayerSnapshot->velocityZ == doctest::Approx(1.5f));
	CHECK(decodedPlayerSnapshot->yawDegrees == doctest::Approx(45.0f));
	CHECK(decodedPlayerSnapshot->pitchDegrees == doctest::Approx(-10.0f));

	serializedPayloadBytes.push_back(std::byte{ 0x7f });
	CHECK(!ve::network::TryDeserializePlayerSnapshot(serializedPayloadBytes).has_value());
}

