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

TEST_CASE("network client hello carries protocol capability flags")
{
	const ve::network::ClientHelloPayload clientHello{
		"Renato",
		ve::network::ProtocolCapabilityBlockMutations
	};
	ve::network::ByteBuffer serializedPayloadBytes = ve::network::SerializeClientHello(clientHello);

	const std::optional<ve::network::ClientHelloPayload> decodedClientHello =
		ve::network::TryDeserializeClientHelloPayload(serializedPayloadBytes);

	REQUIRE(decodedClientHello.has_value());
	CHECK(decodedClientHello->playerName == "Renato");
	CHECK(decodedClientHello->capabilityFlags == ve::network::ProtocolCapabilityBlockMutations);
	CHECK(ve::network::TryDeserializeClientHello(serializedPayloadBytes) == "Renato");

	const std::uint32_t unknownCapabilityFlag = 1U << 31U;
	std::memcpy(serializedPayloadBytes.data() + serializedPayloadBytes.size() - sizeof(unknownCapabilityFlag),
		&unknownCapabilityFlag,
		sizeof(unknownCapabilityFlag));
	CHECK(!ve::network::TryDeserializeClientHelloPayload(serializedPayloadBytes).has_value());
}
