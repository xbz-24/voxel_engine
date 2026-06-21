#pragma once

#include "ChunkMeshTypes.h"
#include "GpuDrivenDrawBuffer.h"

#include <cstdint>
#include <vector>

namespace ve::rendering
{
	/**
	 * Texture/material range after the OpenGL quad batches have been converted to Vulkan triangles.
	 */
	struct VulkanChunkMeshBatch
	{
		TextureHandle texture = kInvalidTextureHandle;
		std::uint32_t first_index = 0;
		std::uint32_t index_count = 0;
	};

	/**
	 * Backend-neutral payload shape needed before uploading chunk meshes to Vulkan buffers.
	 */
	struct VulkanChunkMeshPayload
	{
		std::vector<ChunkVertex> vertices;
		std::vector<std::uint32_t> indices;
		std::vector<VulkanChunkMeshBatch> batches;
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
