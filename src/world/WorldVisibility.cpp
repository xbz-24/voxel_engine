#include "WorldVisibility.h"

#include "ChunkTerrain.h"

#include <glm/ext.hpp>

namespace ve::world::visibility
{
	bool IsChunkInView(const glm::vec3& cameraPosition, const glm::vec3& cameraForward, int chunkX, int chunkZ)
	{
		constexpr float alwaysVisibleDistance = 24.0f;
		constexpr float rearCullCosine = -0.35f;
		const float centerX = static_cast<float>(chunkX * terrain::ChunkWidth) + (terrain::ChunkWidth * 0.5f);
		const float centerZ = static_cast<float>(chunkZ * terrain::ChunkDepth) + (terrain::ChunkDepth * 0.5f);
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
