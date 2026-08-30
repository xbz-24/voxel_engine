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

