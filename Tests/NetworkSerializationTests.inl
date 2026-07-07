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
