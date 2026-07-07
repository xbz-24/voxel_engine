namespace
{
	constexpr std::uint32_t PayloadChecksumOffsetBasis = 2166136261U;
	constexpr std::uint32_t PayloadChecksumPrime = 16777619U;
	constexpr std::uint32_t SerializedPacketHeaderByteCount =
		sizeof(std::uint32_t) + sizeof(std::uint16_t) + sizeof(ve::network::NetworkMessageType) +
		sizeof(std::uint32_t) + sizeof(std::uint32_t) + sizeof(std::uint32_t);
	static_assert(ve::network::PacketHeaderByteCount == SerializedPacketHeaderByteCount);

	bool IsKnownMessageType(ve::network::NetworkMessageType messageType)
	{
		switch (messageType)
		{
		case ve::network::NetworkMessageType::ClientHello:
		case ve::network::NetworkMessageType::ServerWelcome:
		case ve::network::NetworkMessageType::PlayerSnapshot:
		case ve::network::NetworkMessageType::BlockMutation:
		case ve::network::NetworkMessageType::Ping:
		case ve::network::NetworkMessageType::Pong:
		case ve::network::NetworkMessageType::Disconnect:
			return true;
		}
		return false;
	}

	std::uint32_t CalculatePayloadChecksum(std::span<const std::byte> payloadBytes)
	{
		std::uint32_t payloadChecksum = PayloadChecksumOffsetBasis;
		for (const std::byte payloadByte : payloadBytes)
		{
			payloadChecksum ^= std::to_integer<std::uint32_t>(payloadByte);
			payloadChecksum *= PayloadChecksumPrime;
		}
		return payloadChecksum;
	}

	template <typename Value>
	void AppendHeaderField(ve::network::ByteBuffer& packetBytes, const Value& value)
	{
		const auto* firstByte = reinterpret_cast<const std::byte*>(&value);
		packetBytes.insert(packetBytes.end(), firstByte, firstByte + sizeof(Value));
	}

	template <typename Value>
	void ReadHeaderField(std::span<const std::byte> headerBytes, std::size_t& headerByteOffset, Value& output)
	{
		std::memcpy(&output, headerBytes.data() + headerByteOffset, sizeof(Value));
		headerByteOffset += sizeof(Value);
	}
}
