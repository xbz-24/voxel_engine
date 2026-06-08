#include "NetworkSerialization.h"

#include <cstring>

namespace
{
	/// Appends raw bytes for a trivially copyable value.
	template <typename Value>
	void AppendValue(ve::network::ByteBuffer& bytes, const Value& value)
	{
		const auto* firstByte = reinterpret_cast<const std::byte*>(&value);
		bytes.insert(bytes.end(), firstByte, firstByte + sizeof(Value));
	}
}

namespace ve::network
{
	ByteBuffer SerializeClientHello(const std::string& playerName)
	{
		ByteBuffer payloadBytes;
		const std::uint16_t nameByteCount = static_cast<std::uint16_t>(playerName.size());
		AppendValue(payloadBytes, nameByteCount);
		payloadBytes.insert(payloadBytes.end(), reinterpret_cast<const std::byte*>(playerName.data()), reinterpret_cast<const std::byte*>(playerName.data() + playerName.size()));
		return payloadBytes;
	}

	ByteBuffer SerializePlayerSnapshot(const PlayerSnapshotPayload& playerSnapshot)
	{
		ByteBuffer payloadBytes;
		AppendValue(payloadBytes, playerSnapshot);
		return payloadBytes;
	}

	ByteBuffer SerializeBlockMutation(const BlockMutationPayload& blockMutation)
	{
		ByteBuffer payloadBytes;
		AppendValue(payloadBytes, blockMutation);
		return payloadBytes;
	}

	std::optional<std::string> TryDeserializeClientHello(std::span<const std::byte> payloadBytes)
	{
		if (payloadBytes.size() < sizeof(std::uint16_t)) return std::nullopt;
		std::uint16_t nameByteCount = 0;
		std::memcpy(&nameByteCount, payloadBytes.data(), sizeof(nameByteCount));
		if (payloadBytes.size() != sizeof(nameByteCount) + nameByteCount) return std::nullopt;
		const char* firstCharacter = reinterpret_cast<const char*>(payloadBytes.data() + sizeof(nameByteCount));
		return std::string(firstCharacter, firstCharacter + nameByteCount);
	}
}
