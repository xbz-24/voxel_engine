#include "VulkanGpuChunkRenderer.h"

#include "Logger.h"
#include "World.h"

#include <chrono>
#include <string>

namespace ve::rendering
{
	bool VulkanGpuChunkRenderer::UploadMeshBuffers(std::span<const VoxelVertex> vertices, std::span<const std::uint32_t> indices)
	{
		ReleaseMeshBuffers();
		index_count_ = static_cast<std::uint32_t>(indices.size());
		if (vertices.empty() || indices.empty()) return true;
		const VkDeviceSize vertex_bytes = static_cast<VkDeviceSize>(vertices.size_bytes());
		const VkDeviceSize index_bytes = static_cast<VkDeviceSize>(indices.size_bytes());
		const bool uploaded =
			UploadDeviceLocalBuffer(vertices.data(), vertex_bytes, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, vertex_buffer_, vertex_memory_) &&
			UploadDeviceLocalBuffer(indices.data(), index_bytes, VK_BUFFER_USAGE_INDEX_BUFFER_BIT, index_buffer_, index_memory_);
		if (!uploaded) ReleaseMeshBuffers();
		return uploaded;
	}
	bool VulkanGpuChunkRenderer::EnsureWorldMesh(const ve::world::World& world)
	{
		if (!NeedsWorldMeshUpdate(world)) return true;
		// TODO: Surface rebuild/upload timing through renderer stats instead of relying only on debug log output.
		const auto rebuild_start = std::chrono::steady_clock::now();
		std::vector<VoxelVertex> vertices;
		std::vector<std::uint32_t> indices;
		RebuildMesh(world, vertices, indices);
		if (!UploadMeshBuffers(vertices, indices)) return false;
		mesh_revision_ = world.Revision();
		mesh_valid_ = true;
		const auto rebuild_end = std::chrono::steady_clock::now();
		const double rebuild_ms = std::chrono::duration<double, std::milli>(rebuild_end - rebuild_start).count();
		VE_LOG_CATEGORY_DEBUG(ve::log::category::Render, "Rebuilt Vulkan world mesh: " + std::to_string(index_count_) +
			" indices in " + std::to_string(rebuild_ms) + " ms");
		return true;
	}
	bool VulkanGpuChunkRenderer::NeedsWorldMeshUpdate(const ve::world::World& world) const noexcept
	{
		return !mesh_valid_ || mesh_revision_ != world.Revision();
	}
}
