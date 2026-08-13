#pragma once

#include "Block.h"
#include "GameplayInput.h"

#include <glm/vec3.hpp>

namespace ve::blocks
{
	class BlockRegistry;
}

namespace ve::gameplay
{
	struct RuntimeSettings;
}

namespace ve::world
{
	class World;
}

namespace ve::engine::game_controller_detail
{
	[[nodiscard]] bool IsTunnelMaterial(ve::blocks::BlockId block) noexcept;
	[[nodiscard]] bool CarveRoundedHole(
		ve::world::World& world,
		const ve::blocks::BlockRegistry& block_registry,
		const glm::ivec3& tunnel_center);
	void ConsumeFlyToggle(
		const ve::input::InputSnapshot& input,
		ve::gameplay::RuntimeSettings& settings,
		bool& was_pressed);
	void ConsumeRenderDistanceAdjustment(
		const ve::input::InputSnapshot& input,
		ve::gameplay::RuntimeSettings& settings,
		ve::gameplay::GameplayAction action,
		int amount,
		bool& was_pressed);
}
