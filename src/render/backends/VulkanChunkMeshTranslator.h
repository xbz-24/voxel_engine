#pragma once

#include "ChunkMeshTypes.h"
#include "GpuDrivenDrawBuffer.h"

#include <cstdint>
#include <vector>

namespace ve::rendering
{
	/**
	 * Backend-neutral payload shape needed before uploading chunk meshes to Vulkan buffers.
	 */
	struct VulkanChunkMeshPayload
	{
		std::vector<ChunkVertex> vertices;
		std::vector<std::uint32_t> indices;
		GpuDrawElementsCommand draw{};
	};

	/**
	 * Converts legacy OpenGL quad chunk mesh output into Vulkan triangle-indexed geometry.
	 *
	 * @param mesh CPU chunk mesh currently produced for the OpenGL compatibility path.
	 * @return Triangle-indexed payload suitable for a future Vulkan vertex/index upload.
	 */
	[[nodiscard]] VulkanChunkMeshPayload BuildVulkanChunkMeshPayload(const ve::world::mesh::ChunkMeshBuildResult& mesh);
}
