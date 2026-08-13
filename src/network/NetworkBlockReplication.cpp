#include "NetworkBlockReplication.h"

#include "Block.h"
#include "NetworkSerialization.h"
#include "World.h"
#include "WorldBlockEdit.h"

namespace ve::network
{
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
			ve::blocks::ToBlockByte(blockChangedEvent.newBlockId)
		};
		return NetworkMessage{ NetworkMessageType::BlockMutation, SerializeBlockMutation(blockMutation) };
	}

	std::vector<NetworkMessage> BuildBlockMutationMessages(std::span<const ve::world::WorldEvent> worldEvents)
	{
		std::vector<NetworkMessage> networkMessages;
		networkMessages.reserve(worldEvents.size());
		for (const ve::world::WorldEvent& worldEvent : worldEvents)
		{
			if (const ve::world::BlockChangedEvent* blockChangedEvent = worldEvent.AsBlockChanged())
			{
				networkMessages.push_back(BuildBlockMutationMessage(*blockChangedEvent));
			}
		}
		return networkMessages;
	}

	bool ApplyNetworkBlockMutation(ve::world::World& world, const NetworkMessage& message)
	{
		const std::optional<ve::gameplay::BlockInteraction> blockInteraction = TryReadBlockMutationMessage(message);
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
