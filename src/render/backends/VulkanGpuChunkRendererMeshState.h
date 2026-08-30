#pragma once

#include "VulkanGpuChunkRendererVertexTypes.h"

#include <cstdint>
#include <limits>
#include <vector>

namespace ve::rendering
{
	/** Diagnostics for the most recent GPU chunk mesh rebuild and upload. */
	struct VulkanGpuChunkMeshStats
	{
		double last_rebuild_cpu_ms = 0.0;
		double last_upload_cpu_ms = 0.0;
		std::uint32_t last_vertex_count = 0;
		std::uint32_t last_index_count = 0;
		std::uint32_t last_shadow_index_count = 0;
		std::uint32_t last_rebuilt_chunk_count = 0;
		std::uint32_t cached_chunk_count = 0;
		VkDeviceSize vertex_buffer_capacity_bytes = 0;
		VkDeviceSize index_buffer_capacity_bytes = 0;
		VkDeviceSize shadow_index_buffer_capacity_bytes = 0;
	};

	struct VulkanGpuCachedChunkMesh
	{
		int chunk_x = 0;
		int chunk_z = 0;
		std::uint64_t mesh_revision = std::numeric_limits<std::uint64_t>::max();
		std::vector<VoxelVertex> vertices;
		std::vector<std::uint32_t> indices;
	};

	struct VulkanGpuChunkRendererMeshState
	{
		VkBuffer vertex_buffer_ = VK_NULL_HANDLE;
		VkDeviceMemory vertex_memory_ = VK_NULL_HANDLE;
		VkDeviceSize vertex_buffer_capacity_bytes_ = 0;
		VkBuffer index_buffer_ = VK_NULL_HANDLE;
		VkDeviceMemory index_memory_ = VK_NULL_HANDLE;
		VkDeviceSize index_buffer_capacity_bytes_ = 0;
		VkBuffer shadow_index_buffer_ = VK_NULL_HANDLE;
		VkDeviceMemory shadow_index_memory_ = VK_NULL_HANDLE;
		VkDeviceSize shadow_index_buffer_capacity_bytes_ = 0;
		std::uint64_t mesh_revision_ = 0;
		std::uint64_t cached_chunk_storage_revision_ = 0;
		std::uint32_t index_count_ = 0;
		std::uint32_t shadow_index_count_ = 0;
		std::uint32_t last_rebuilt_chunk_count_ = 0;
		VulkanGpuChunkMeshStats mesh_stats_;
		std::vector<VulkanGpuCachedChunkMesh> cached_chunk_meshes_;
		bool mesh_valid_ = false;
	};
}
