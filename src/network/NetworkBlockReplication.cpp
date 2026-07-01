#include "NetworkBlockReplication.h"

#include "NetworkSerialization.h"
#include "World.h"
#include "WorldBlockEdit.h"

#include <cstdint>

namespace
{
	/// Returns true when a byte maps to a concrete block id.
	bool IsReplicatedBlockIdValid(std::uint8_t blockIdByte)
	{
		return blockIdByte < static_cast<std::uint8_t>(ve::blocks::BlockId::Count);
	}

	/// Builds a world position from a protocol payload.
	glm::ivec3 ToBlockPosition(const ve::network::BlockMutationPayload& blockMutation)
	{
		return glm::ivec3(blockMutation.blockX, blockMutation.blockY, blockMutation.blockZ);
	}

	/// Converts a protocol byte into an engine block id.
	ve::blocks::BlockId ToBlockId(const ve::network::BlockMutationPayload& blockMutation)
	{
		return static_cast<ve::blocks::BlockId>(blockMutation.blockId);
	}

	/// Chooses the gameplay interaction represented by a replicated block id.
	ve::gameplay::BlockInteractionType ToInteractionType(ve::blocks::BlockId blockId)
	{
		if (blockId == ve::blocks::BlockId::Air) return ve::gameplay::BlockInteractionType::Break;
		return ve::gameplay::BlockInteractionType::Place;
	}

	/// Converts decoded protocol data into gameplay data.
	ve::gameplay::BlockInteraction ToBlockInteraction(const ve::network::BlockMutationPayload& blockMutation)
	{
		const ve::blocks::BlockId targetBlockId = ToBlockId(blockMutation);
		return ve::gameplay::BlockInteraction{
			ToInteractionType(targetBlockId),
			ToBlockPosition(blockMutation),
			targetBlockId
		};
	}
}

namespace ve::network
{
	/// Builds a network message from a local block-change event.
	NetworkMessage BuildBlockMutationMessage(const ve::world::BlockChangedEvent& blockChangedEvent)
	{
		constexpr std::uint32_t untrackedMutationId = 0;
		constexpr std::uint32_t localAuthorPlayerId = 0;
		const BlockMutationPayload blockMutation{
			untrackedMutationId,
			localAuthorPlayerId,
			blockChangedEvent.worldBlockPosition.x,
			blockChangedEvent.worldBlockPosition.y,
			blockChangedEvent.worldBlockPosition.z,
			static_cast<std::uint8_t>(blockChangedEvent.newBlockId)
		};
		return NetworkMessage{ NetworkMessageType::BlockMutation, SerializeBlockMutation(blockMutation) };
	}

	/// Builds network messages from block-change world events.
	std::vector<NetworkMessage> BuildBlockMutationMessages(std::span<const ve::world::WorldEvent> worldEvents)
	{
		std::vector<NetworkMessage> networkMessages;
		networkMessages.reserve(worldEvents.size());
		for (const ve::world::WorldEvent& worldEvent : worldEvents)
		{
			if (worldEvent.eventType == ve::world::WorldEventType::BlockChanged)
			{
				networkMessages.push_back(BuildBlockMutationMessage(worldEvent.blockChanged));
			}
		}
		return networkMessages;
	}

	/// Converts a BlockMutation message into gameplay data.
	std::optional<ve::gameplay::BlockInteraction> TryReadBlockMutationMessage(const NetworkMessage& message)
	{
		if (message.messageType != NetworkMessageType::BlockMutation) return std::nullopt;
		std::optional<BlockMutationPayload> blockMutation = TryDeserializeBlockMutation(message.payloadBytes);
		if (!blockMutation || !IsReplicatedBlockIdValid(blockMutation->blockId)) return std::nullopt;
		return ToBlockInteraction(*blockMutation);
	}

	bool ApplyNetworkBlockMutation(ve::world::World& world, const NetworkMessage& message)
	{
		std::optional<ve::gameplay::BlockInteraction> blockInteraction = TryReadBlockMutationMessage(message);
		if (!blockInteraction) return false;
		return ApplyNetworkBlockMutation(world, *blockInteraction);
	}

	bool ApplyNetworkBlockMutation(
		ve::world::World& world,
		const ve::gameplay::BlockInteraction& blockInteraction)
	{
		return ve::world::ApplyWorldBlockEdit(world,
			ve::world::MakeSingleBlockEdit(
				blockInteraction.position.x,
				blockInteraction.position.y,
				blockInteraction.position.z,
				blockInteraction.blockId));
	}
}
