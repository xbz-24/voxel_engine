#include "WorldVisibility.h"

#include "WorldGridMath.h"

#include <glm/ext.hpp>

namespace ve::world::visibility
{
	bool IsChunkInView(const glm::vec3& cameraPosition, const glm::vec3& cameraForward, int chunkX, int chunkZ)
	{
		constexpr float alwaysVisibleDistance = 24.0f;
		constexpr float rearCullCosine = -0.35f;
		const float centerX = grid::ChunkCenterX(chunkX);
		const float centerZ = grid::ChunkCenterZ(chunkZ);
		glm::vec3 toChunk(centerX - cameraPosition.x, 0.0f, centerZ - cameraPosition.z);
		if (glm::length(toChunk) <= alwaysVisibleDistance)
		{
			return true;
		}

		glm::vec3 forward(cameraForward.x, 0.0f, cameraForward.z);
		if (glm::length(forward) <= 0.001f)
		{
			return true;
		}

		toChunk = glm::normalize(toChunk);
		forward = glm::normalize(forward);
		return glm::dot(toChunk, forward) >= rearCullCosine;
	}
}
