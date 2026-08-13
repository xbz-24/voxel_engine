#pragma once

namespace ve::blocks
{
	class BlockRegistry;
}

namespace ve::gameplay
{
	struct BlockSelection;
	struct RuntimeSettings;
}

namespace ve::input
{
	struct InputSnapshot;
}

namespace ve::world
{
	class World;
}

class Camera;

namespace ve::engine::game_controller_detail
{
	struct PlayerMovementFrameContext
	{
		PlayerMovementFrameContext(const ve::input::InputSnapshot& input,
			ve::world::World& world,
			const ve::blocks::BlockRegistry& block_registry,
			::Camera& camera,
			ve::gameplay::RuntimeSettings& settings,
			double delta_seconds) noexcept
			: input(input), world(world), block_registry(block_registry), camera(camera),
			  settings(settings), delta_seconds(delta_seconds) {}

		const ve::input::InputSnapshot& input;
		ve::world::World& world;
		const ve::blocks::BlockRegistry& block_registry;
		::Camera& camera;
		ve::gameplay::RuntimeSettings& settings;
		double delta_seconds;
	};

	struct BlockSelectionFrameContext
	{
		BlockSelectionFrameContext(ve::world::World& world,
			const ve::blocks::BlockRegistry& block_registry,
			::Camera& camera,
			ve::gameplay::BlockSelection& selection) noexcept
			: world(world), block_registry(block_registry), camera(camera), selection(selection) {}

		ve::world::World& world;
		const ve::blocks::BlockRegistry& block_registry;
		::Camera& camera;
		ve::gameplay::BlockSelection& selection;
	};

	struct GameplayCommandFrameContext
	{
		GameplayCommandFrameContext(const ve::input::InputSnapshot& input,
			ve::world::World& world,
			ve::gameplay::BlockSelection& selection,
			ve::gameplay::RuntimeSettings& settings) noexcept
			: input(input), world(world), selection(selection), settings(settings) {}

		const ve::input::InputSnapshot& input;
		ve::world::World& world;
		ve::gameplay::BlockSelection& selection;
		ve::gameplay::RuntimeSettings& settings;
	};
}
