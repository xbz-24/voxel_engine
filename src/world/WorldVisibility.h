#pragma once

#include <glm/glm.hpp>

namespace ve::world::visibility
{
	/**
	 * Checks whether a chunk is close enough to the camera view direction to draw.
	 *
	 * @param cameraPosition Camera world position.
	 * @param cameraForward Camera forward direction.
	 * @param chunkX Chunk-grid X coordinate.
	 * @param chunkZ Chunk-grid Z coordinate.
	 * @return True when the chunk should be submitted for rendering.
	 */
	bool IsChunkInView(const glm::vec3& cameraPosition, const glm::vec3& cameraForward, int chunkX, int chunkZ);
}
