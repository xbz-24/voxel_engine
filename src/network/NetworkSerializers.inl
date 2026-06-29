namespace ve::network
{
	ByteBuffer SerializeClientHello(const std::string& playerName)
	{
		PayloadWriter writer;
		const std::size_t bounded_size = std::min(playerName.size(), static_cast<std::size_t>(MaxPlayerNameByteCount));
		const std::uint16_t nameByteCount = static_cast<std::uint16_t>(bounded_size);
		writer.Write(nameByteCount);
		writer.WriteBytes(std::as_bytes(std::span(playerName.data(), bounded_size)));
		return std::move(writer).Finish();
	}

	ByteBuffer SerializePlayerSnapshot(const PlayerSnapshotPayload& playerSnapshot)
	{
		PayloadWriter writer;
		writer.Write(playerSnapshot);
		return std::move(writer).Finish();
	}

	ByteBuffer SerializeBlockMutation(const BlockMutationPayload& blockMutation)
	{
		PayloadWriter writer;
		writer.Write(blockMutation.blockX);
		writer.Write(blockMutation.blockY);
		writer.Write(blockMutation.blockZ);
		writer.Write(blockMutation.blockId);
		return std::move(writer).Finish();
	}
