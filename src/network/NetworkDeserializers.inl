	std::optional<std::string> TryDeserializeClientHello(std::span<const std::byte> payloadBytes)
	{
		if (payloadBytes.size() < sizeof(std::uint16_t)) return std::nullopt;
		std::uint16_t nameByteCount = 0;
		std::memcpy(&nameByteCount, payloadBytes.data(), sizeof(nameByteCount));
		if (nameByteCount == 0 || nameByteCount > MaxPlayerNameByteCount) return std::nullopt;
		if (payloadBytes.size() != sizeof(nameByteCount) + nameByteCount) return std::nullopt;
		const char* firstCharacter = reinterpret_cast<const char*>(payloadBytes.data() + sizeof(nameByteCount));
		return std::string(firstCharacter, firstCharacter + nameByteCount);
	}

	std::optional<PlayerSnapshotPayload> TryDeserializePlayerSnapshot(std::span<const std::byte> payloadBytes)
	{
		PayloadReader reader(payloadBytes);
		PlayerSnapshotPayload playerSnapshot{};
		if (!reader.Read(playerSnapshot.playerId)) return std::nullopt;
		if (!reader.Read(playerSnapshot.simulationTickId)) return std::nullopt;
		if (!reader.Read(playerSnapshot.positionX)) return std::nullopt;
		if (!reader.Read(playerSnapshot.positionY)) return std::nullopt;
		if (!reader.Read(playerSnapshot.positionZ)) return std::nullopt;
		if (!reader.Read(playerSnapshot.velocityX)) return std::nullopt;
		if (!reader.Read(playerSnapshot.velocityY)) return std::nullopt;
		if (!reader.Read(playerSnapshot.velocityZ)) return std::nullopt;
		if (!reader.Read(playerSnapshot.yawDegrees)) return std::nullopt;
		if (!reader.Read(playerSnapshot.pitchDegrees)) return std::nullopt;
		if (!reader.IsFinished()) return std::nullopt;
		return playerSnapshot;
	}

	std::optional<BlockMutationPayload> TryDeserializeBlockMutation(std::span<const std::byte> payloadBytes)
	{
		PayloadReader reader(payloadBytes);
		BlockMutationPayload blockMutation{};
		if (!reader.Read(blockMutation.mutationId)) return std::nullopt;
		if (!reader.Read(blockMutation.authorPlayerId)) return std::nullopt;
		if (!reader.Read(blockMutation.blockX)) return std::nullopt;
		if (!reader.Read(blockMutation.blockY)) return std::nullopt;
		if (!reader.Read(blockMutation.blockZ)) return std::nullopt;
		if (!reader.Read(blockMutation.blockId)) return std::nullopt;
		if (!reader.IsFinished()) return std::nullopt;
		return blockMutation;
	}
}
