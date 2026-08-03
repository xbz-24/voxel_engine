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
