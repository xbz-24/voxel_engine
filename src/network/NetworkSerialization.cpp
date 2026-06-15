#include "NetworkSerialization.h"

#include <cstring>
#include <type_traits>

namespace
{
	/**
	 * Appends bytes for a trivially copyable field.
	 *
	 * @param bytes Destination payload buffer.
	 * @param value Field copied into the payload.
	 */
	template <typename Value>
	void AppendValue(ve::network::ByteBuffer& bytes, const Value& value)
	{
		static_assert(std::is_trivially_copyable_v<Value>);
		const auto* firstByte = reinterpret_cast<const std::byte*>(&value);
		bytes.insert(bytes.end(), firstByte, firstByte + sizeof(Value));
	}

	/**
	 * Reads one trivially copyable field from a payload.
	 *
	 * @param payloadBytes Source payload bytes.
	 * @param readOffset Current byte offset, advanced on success.
	 * @param outputValue Destination field filled from bytes.
	 * @return True when enough bytes were available.
	 */
	template <typename Value>
	bool TryReadValue(std::span<const std::byte> payloadBytes, std::size_t& readOffset, Value& outputValue)
	{
		static_assert(std::is_trivially_copyable_v<Value>);
		if (readOffset > payloadBytes.size()) return false;
		if (payloadBytes.size() - readOffset < sizeof(Value)) return false;
		std::memcpy(&outputValue, payloadBytes.data() + readOffset, sizeof(Value));
		readOffset += sizeof(Value);
		return true;
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
		AppendValue(payloadBytes, blockMutation.blockX);
		AppendValue(payloadBytes, blockMutation.blockY);
		AppendValue(payloadBytes, blockMutation.blockZ);
		AppendValue(payloadBytes, blockMutation.blockId);
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

	std::optional<PlayerSnapshotPayload> TryDeserializePlayerSnapshot(std::span<const std::byte> payloadBytes)
	{
		if (payloadBytes.size() != sizeof(PlayerSnapshotPayload)) return std::nullopt;
		PlayerSnapshotPayload playerSnapshot{};
		std::memcpy(&playerSnapshot, payloadBytes.data(), sizeof(playerSnapshot));
		return playerSnapshot;
	}

	std::optional<BlockMutationPayload> TryDeserializeBlockMutation(std::span<const std::byte> payloadBytes)
	{
		std::size_t readOffset = 0;
		BlockMutationPayload blockMutation{};
		if (!TryReadValue(payloadBytes, readOffset, blockMutation.blockX)) return std::nullopt;
		if (!TryReadValue(payloadBytes, readOffset, blockMutation.blockY)) return std::nullopt;
		if (!TryReadValue(payloadBytes, readOffset, blockMutation.blockZ)) return std::nullopt;
		if (!TryReadValue(payloadBytes, readOffset, blockMutation.blockId)) return std::nullopt;
		if (readOffset != payloadBytes.size()) return std::nullopt;
		return blockMutation;
	}
}
