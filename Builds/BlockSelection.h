#pragma once

#include <glm/glm.hpp>

namespace ve::gameplay
{
	struct BlockSelection
	{
		bool has_target = false;
		glm::ivec3 target_block{ 0 };
		glm::ivec3 placement_block{ 0 };
	};
}
