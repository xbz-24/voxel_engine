#include "NetworkBlockReplication.h"

#include "Block.h"
#include "NetworkSerialization.h"

namespace
{
	glm::ivec3 ToBlockPosition(const ve::network::BlockMutationPayload& mutation)
	{
		return glm::ivec3(mutation.blockX, mutation.blockY, mutation.blockZ);
	}

	ve::blocks::BlockId ToBlockId(const ve::network::BlockMutationPayload& mutation)
	{
		return ve::blocks::BlockIdFromByte(mutation.blockId);
	}

	ve::gameplay::BlockInteractionType ToInteractionType(ve::blocks::BlockId blockId)
	{
		return blockId == ve::blocks::BlockId::Air
			? ve::gameplay::BlockInteractionType::Break
			: ve::gameplay::BlockInteractionType::Place;
	}

	ve::gameplay::BlockInteraction ToBlockInteraction(const ve::network::BlockMutationPayload& mutation)
	{
		const ve::blocks::BlockId blockId = ToBlockId(mutation);
		return { ToInteractionType(blockId), ToBlockPosition(mutation), blockId };
	}
}

namespace ve::network
{
	std::optional<ve::gameplay::BlockInteraction> TryReadBlockMutationMessage(const NetworkMessage& message)
	{
		if (message.messageType != NetworkMessageType::BlockMutation) return std::nullopt;
		const std::optional<BlockMutationPayload> mutation = TryDeserializeBlockMutation(message.payloadBytes);
		if (!mutation || !ve::blocks::IsStoredBlockByte(mutation->blockId)) return std::nullopt;
		return ToBlockInteraction(*mutation);
	}
}
