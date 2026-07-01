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
			: block_registry(registry),
			  camera_position(camera_position),
			  camera_forward(camera_forward),
			  view_projection(view_projection_matrix),
			  render_distance_chunks(render_distance_chunks)
		{
		}

		const ve::blocks::BlockRegistry& block_registry;
		glm::vec3 camera_position;
		glm::vec3 camera_forward;
		glm::mat4 view_projection;
		int render_distance_chunks;
	};

	/** Backend-neutral visible chunk render payload. */
	struct ChunkRenderItem
	{
		const ve::rendering::ChunkGpuMesh* mesh = nullptr;
		int chunk_x = 0;
		int chunk_z = 0;
	};

	using ChunkRenderItemList = std::vector<ChunkRenderItem>;
}
