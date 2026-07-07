#include "VulkanGpuChunkRenderer.h"

#include "Logger.h"
#include "World.h"

#include <chrono>
#include <string>

namespace ve::rendering
{
	bool VulkanGpuChunkRenderer::UploadMeshBuffers(std::span<const VoxelVertex> vertices, std::span<const std::uint32_t> indices)
	{
		index_count_ = static_cast<std::uint32_t>(indices.size());
		if (vertices.empty() || indices.empty()) return true;
		const VkDeviceSize vertex_bytes = static_cast<VkDeviceSize>(vertices.size_bytes());
		const VkDeviceSize index_bytes = static_cast<VkDeviceSize>(indices.size_bytes());
		const bool uploaded = UploadDeviceLocalBuffer(
			vertices.data(),
			vertex_bytes,
			VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
			vertex_buffer_,
			vertex_memory_,
			vertex_buffer_capacity_bytes_) &&
			UploadDeviceLocalBuffer(
				indices.data(),
				index_bytes,
				VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
				index_buffer_,
				index_memory_,
				index_buffer_capacity_bytes_);
		if (!uploaded) ReleaseMeshBuffers();
		return uploaded;
	}
	bool VulkanGpuChunkRenderer::EnsureWorldMesh(const ve::world::World& world, const ve::blocks::BlockRegistry& block_registry)
	{
		if (!NeedsWorldMeshUpdate(world)) return true;
		const auto rebuild_start = std::chrono::steady_clock::now();
		std::vector<VoxelVertex> vertices;
		std::vector<std::uint32_t> indices;
		RebuildMesh(world, block_registry, vertices, indices);
		const auto rebuild_end = std::chrono::steady_clock::now();
		const auto upload_start = std::chrono::steady_clock::now();
		if (!UploadMeshBuffers(vertices, indices)) return false;
		const auto upload_end = std::chrono::steady_clock::now();
		mesh_revision_ = world.Revision();
		mesh_valid_ = true;
		const double rebuild_ms = std::chrono::duration<double, std::milli>(rebuild_end - rebuild_start).count();
		const double upload_ms = std::chrono::duration<double, std::milli>(upload_end - upload_start).count();
		mesh_stats_ = VulkanGpuChunkMeshStats{
			rebuild_ms,
			upload_ms,
			static_cast<std::uint32_t>(vertices.size()),
			index_count_,
			last_rebuilt_chunk_count_,
			static_cast<std::uint32_t>(cached_chunk_meshes_.size()),
			vertex_buffer_capacity_bytes_,
			index_buffer_capacity_bytes_
		};
		VE_LOG_CATEGORY_DEBUG(ve::log::category::Render, "Rebuilt Vulkan world mesh: " + std::to_string(index_count_) +
			" indices, " + std::to_string(last_rebuilt_chunk_count_) + "/" +
			std::to_string(cached_chunk_meshes_.size()) + " chunks rebuilt in " +
			std::to_string(rebuild_ms + upload_ms) + " ms");
		return true;
	}
	bool VulkanGpuChunkRenderer::NeedsWorldMeshUpdate(const ve::world::World& world) const noexcept
	{
		return !mesh_valid_ || mesh_revision_ != world.Revision();
	}
}
