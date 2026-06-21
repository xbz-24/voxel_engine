#pragma once

#include <glm/glm.hpp>
#include <glm/mat4x4.hpp>
#include <vector>

namespace ve::rendering
{
	class ChunkGpuMesh;
}

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
		WorldRenderRequest(const ve::blocks::BlockRegistry& registry,
			glm::vec3 camera_position,
			glm::vec3 camera_forward,
			glm::mat4 view_projection_matrix,
			int render_distance_chunks) noexcept
			: blockRegistry(registry),
			  cameraPosition(camera_position),
			  cameraForward(camera_forward),
			  viewProjection(view_projection_matrix),
			  renderDistanceChunks(render_distance_chunks)
		{
		}

		const ve::blocks::BlockRegistry& blockRegistry;
		glm::vec3 cameraPosition;
		glm::vec3 cameraForward;
		glm::mat4 viewProjection;
		int renderDistanceChunks;
	};

	/** Backend-neutral visible chunk render payload. */
	struct ChunkRenderItem
	{
		const ve::rendering::ChunkGpuMesh* mesh = nullptr;
		int chunkX = 0;
		int chunkZ = 0;
	};

	using ChunkRenderItemList = std::vector<ChunkRenderItem>;
}
