namespace ve::network
{
	ByteBuffer SerializeClientHello(const std::string& playerName)
	{
		PayloadWriter writer;
		const std::size_t boundedNameByteCount = std::min(playerName.size(), static_cast<std::size_t>(MaxPlayerNameByteCount));
		const std::uint16_t nameByteCount = static_cast<std::uint16_t>(boundedNameByteCount);
		writer.Write(nameByteCount);
		writer.WriteBytes(std::as_bytes(std::span(playerName.data(), boundedNameByteCount)));
		return std::move(writer).Finish();
	}

	ByteBuffer SerializePlayerSnapshot(const PlayerSnapshotPayload& playerSnapshot)
	{
		PayloadWriter writer;
		writer.Write(playerSnapshot.playerId);
		writer.Write(playerSnapshot.simulationTickId);
		writer.Write(playerSnapshot.positionX);
		writer.Write(playerSnapshot.positionY);
		writer.Write(playerSnapshot.positionZ);
		writer.Write(playerSnapshot.velocityX);
		writer.Write(playerSnapshot.velocityY);
		writer.Write(playerSnapshot.velocityZ);
		writer.Write(playerSnapshot.yawDegrees);
		writer.Write(playerSnapshot.pitchDegrees);
		return std::move(writer).Finish();
	}

	ByteBuffer SerializeBlockMutation(const BlockMutationPayload& blockMutation)
	{
		PayloadWriter writer;
		writer.Write(blockMutation.mutationId);
		writer.Write(blockMutation.authorPlayerId);
		writer.Write(blockMutation.blockX);
		writer.Write(blockMutation.blockY);
		writer.Write(blockMutation.blockZ);
		writer.Write(blockMutation.blockId);
		return std::move(writer).Finish();
	}
