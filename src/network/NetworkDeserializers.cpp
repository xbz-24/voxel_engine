#include "NetworkSerialization.h"

#include "NetworkByteCodec.h"
#include "NetworkPayloadCodec.h"

namespace ve::network
{
	std::optional<std::string> TryDeserializeClientHello(std::span<const std::byte> payloadBytes)
	{
		const std::optional<ClientHelloPayload> decodedClientHello = TryDeserializeClientHelloPayload(payloadBytes);
		if (!decodedClientHello) return std::nullopt;
		return decodedClientHello->playerName;
	}

	std::optional<ClientHelloPayload> TryDeserializeClientHelloPayload(std::span<const std::byte> payloadBytes)
	{
		detail::PayloadReader reader(payloadBytes);
		std::uint16_t nameByteCount = 0;
		if (!reader.Read(nameByteCount)) return std::nullopt;
		if (nameByteCount == 0 || nameByteCount > MaxPlayerNameByteCount) return std::nullopt;
		const std::optional<std::span<const std::byte>> nameBytes = reader.ReadBytes(nameByteCount);
		if (!nameBytes.has_value()) return std::nullopt;
		std::uint32_t capabilityFlags = 0;
		if (!reader.Read(capabilityFlags)) return std::nullopt;
		if (!reader.IsFinished()) return std::nullopt;
		if ((capabilityFlags & ~SupportedProtocolCapabilityFlags) != 0U) return std::nullopt;
		if ((capabilityFlags & SupportedProtocolCapabilityFlags) == 0U) return std::nullopt;
		return ClientHelloPayload{ ByteSpanToString(*nameBytes), capabilityFlags };
	}

	std::optional<PlayerSnapshotPayload> TryDeserializePlayerSnapshot(std::span<const std::byte> payloadBytes)
	{
		detail::PayloadReader reader(payloadBytes);
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
		detail::PayloadReader reader(payloadBytes);
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
