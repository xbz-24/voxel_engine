#include "NetworkSerialization.h"

#include <cstring>
#include <type_traits>
#include <utility>

namespace
{
	class PayloadWriter
	{
	public:
		template <typename Value>
		void Write(const Value& value)
		{
			static_assert(std::is_trivially_copyable_v<Value>);
			const auto* first_byte = reinterpret_cast<const std::byte*>(&value);
			bytes_.insert(bytes_.end(), first_byte, first_byte + sizeof(Value));
		}

		void WriteBytes(std::span<const std::byte> bytes)
		{
			bytes_.insert(bytes_.end(), bytes.begin(), bytes.end());
		}

		[[nodiscard]] ve::network::ByteBuffer Finish() &&
		{
			return std::move(bytes_);
		}

	private:
		ve::network::ByteBuffer bytes_;
	};

	class PayloadReader
	{
	public:
		explicit PayloadReader(std::span<const std::byte> bytes) noexcept
			: bytes_(bytes)
		{
		}

		template <typename Value>
		bool Read(Value& output)
		{
			static_assert(std::is_trivially_copyable_v<Value>);
			if (offset_ > bytes_.size()) return false;
			if (bytes_.size() - offset_ < sizeof(Value)) return false;
			std::memcpy(&output, bytes_.data() + offset_, sizeof(Value));
			offset_ += sizeof(Value);
			return true;
		}

		[[nodiscard]] bool IsFinished() const noexcept
		{
			return offset_ == bytes_.size();
		}

	private:
		std::span<const std::byte> bytes_;
		std::size_t offset_ = 0;
	};
}

namespace ve::network
{
	ByteBuffer SerializeClientHello(const std::string& playerName)
	{
		PayloadWriter writer;
		const std::uint16_t nameByteCount = static_cast<std::uint16_t>(playerName.size());
		writer.Write(nameByteCount);
		writer.WriteBytes(std::as_bytes(std::span(playerName.data(), playerName.size())));
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
		PayloadReader reader(payloadBytes);
		BlockMutationPayload blockMutation{};
		if (!reader.Read(blockMutation.blockX)) return std::nullopt;
		if (!reader.Read(blockMutation.blockY)) return std::nullopt;
		if (!reader.Read(blockMutation.blockZ)) return std::nullopt;
		if (!reader.Read(blockMutation.blockId)) return std::nullopt;
		if (!reader.IsFinished()) return std::nullopt;
		return blockMutation;
	}
}
