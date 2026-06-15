#pragma once

#include "BlockInteraction.h"
#include "NetworkTypes.h"
#include "WorldEvent.h"

#include <optional>
#include <span>
#include <vector>

namespace ve::world
{
	class World;
}

namespace ve::network
{
	/**
	 * Builds a network message from a local block-change event.
	 *
	 * @param blockChangedEvent World event produced after a block write.
	 * @return Serialized BlockMutation message ready for TCP packet framing.
	 */
	NetworkMessage BuildBlockMutationMessage(const ve::world::BlockChangedEvent& blockChangedEvent);

	/**
	 * Builds network messages from block-change world events.
	 *
	 * @param worldEvents Events drained from a world update.
	 * @return Serialized BlockMutation messages, ignoring non-block events.
	 */
	std::vector<NetworkMessage> BuildBlockMutationMessages(std::span<const ve::world::WorldEvent> worldEvents);

	/**
	 * Converts a BlockMutation message into gameplay data.
	 *
	 * @param message Network message received from a peer.
	 * @return Block interaction, or empty when the type/payload/block id is invalid.
	 */
	std::optional<ve::gameplay::BlockInteraction> TryReadBlockMutationMessage(const NetworkMessage& message);

	/**
	 * Applies a received BlockMutation message to the world.
	 *
	 * @param world World receiving the remote block edit.
	 * @param message Network message received from a peer.
	 * @return True when the message was valid and the world accepted the edit.
	 */
	bool ApplyNetworkBlockMutation(ve::world::World& world, const NetworkMessage& message);
}
