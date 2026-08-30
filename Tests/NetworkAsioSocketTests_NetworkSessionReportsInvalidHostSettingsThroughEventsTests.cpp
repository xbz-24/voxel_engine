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

TEST_CASE("network session reports invalid host settings through events")
{
	ve::network::NetworkSession session;
	ve::network::NetworkHostSettings settings;
	settings.simulationTickRateHz = 0;

	CHECK(!session.HostGame(settings));
	CHECK(session.LastError() == ve::network::NetworkSessionError::InvalidHostTickRate);
	std::vector<ve::network::NetworkSessionEvent> events = session.DrainEvents();
	REQUIRE(events.size() == 1U);
	CHECK(events.front().eventType == ve::network::NetworkSessionEventType::HostStartFailed);
	CHECK(events.front().mode == ve::network::NetworkSessionMode::Offline);
	CHECK(events.front().error == ve::network::NetworkSessionError::InvalidHostTickRate);
	CHECK(session.DrainEvents().empty());
}
