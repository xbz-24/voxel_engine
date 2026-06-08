#pragma once

#include <glm/glm.hpp>

namespace ve::blocks
{
	class BlockRegistry;
}

namespace ve::world
{
	/**
	 * Describes the data needed to submit visible chunks for rendering.
	 */
	struct WorldRenderRequest
	{
		const ve::blocks::BlockRegistry& blockRegistry;
		glm::vec3 cameraPosition;
		glm::vec3 cameraForward;
		int renderDistanceChunks;
	};
}
