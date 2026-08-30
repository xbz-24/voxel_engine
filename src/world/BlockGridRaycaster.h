#pragma once

#include "BlockRaycaster.h"

namespace ve::gameplay::detail
{
	[[nodiscard]] glm::ivec3 FloorToBlock(const glm::vec3& value);
	[[nodiscard]] int StepFor(float direction) noexcept;
	[[nodiscard]] float InitialAxisDistance(float origin, float direction, int block, int step) noexcept;
	[[nodiscard]] float AxisDelta(float direction, int step) noexcept;

	class BlockGridRaycaster
	{
	public:
		BlockGridRaycaster(const ve::world::World& world,
			const ve::blocks::BlockRegistry& block_registry,
			const glm::vec3& origin,
			const glm::vec3& direction,
			float max_distance);

		[[nodiscard]] std::optional<BlockRaycastHit> Trace();

	private:
		[[nodiscard]] float StepToNextBlock();

		const ve::world::World& world_;
		const ve::blocks::BlockRegistry& block_registry_;
		float max_distance_;
		glm::vec3 ray_direction_;
		glm::ivec3 current_block_;
		glm::ivec3 previous_block_;
		glm::ivec3 step_;
		glm::vec3 next_distance_;
		glm::vec3 delta_distance_;
	};
}
