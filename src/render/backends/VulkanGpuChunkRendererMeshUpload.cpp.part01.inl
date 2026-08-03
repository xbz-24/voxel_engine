#include "VulkanGpuChunkRenderer.h"

#include "Logger.h"
#include "World.h"

#include <chrono>
#include <string>

namespace ve::rendering
{
	namespace
	{
		[[nodiscard]] std::vector<std::uint32_t> BuildDirectionalShadowIndices(
			std::span<const VoxelVertex> vertices,
			std::span<const std::uint32_t> indices,
			glm::vec3 sun_direction)
		{
			std::vector<std::uint32_t> shadow_indices;
			shadow_indices.reserve(indices.size() / 2u);
			sun_direction = NormalizeVulkanShaderDirection(
				sun_direction,
				{ -0.42f, 0.78f, -0.46f });
			for (std::size_t offset = 0; offset + 2u < indices.size(); offset += 3u)
			{
				const std::uint32_t first_vertex_index = indices[offset];
				if (first_vertex_index >= vertices.size()) continue;
				const glm::vec3 face_normal = UnpackVoxelNormal(vertices[first_vertex_index].normal_snorm8);
				if (glm::dot(face_normal, sun_direction) <= 0.001f) continue;
				shadow_indices.push_back(indices[offset]);
				shadow_indices.push_back(indices[offset + 1u]);
				shadow_indices.push_back(indices[offset + 2u]);
			}
			return shadow_indices;
		}
	}

	bool VulkanGpuChunkRenderer::UploadMeshBuffers(
		std::span<const VoxelVertex> vertices,
		std::span<const std::uint32_t> indices,
		std::span<const std::uint32_t> shadow_indices)
	{
		index_count_ = RenderElementCount(indices.size());
		shadow_index_count_ = RenderElementCount(shadow_indices.size());
		if (vertices.empty() || indices.empty()) return true;
		const VkDeviceSize vertex_bytes = VulkanByteSize(vertices.size_bytes());
		const VkDeviceSize index_bytes = VulkanByteSize(indices.size_bytes());
		const VkDeviceSize shadow_index_bytes = VulkanByteSize(shadow_indices.size_bytes());
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
				index_buffer_capacity_bytes_) &&
			(shadow_indices.empty() || UploadDeviceLocalBuffer(
				shadow_indices.data(),
				shadow_index_bytes,
				VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
				shadow_index_buffer_,
				shadow_index_memory_,
				shadow_index_buffer_capacity_bytes_));
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
		const std::vector<std::uint32_t> shadow_indices = BuildDirectionalShadowIndices(
			vertices,
			indices,
			render_style_.sun_direction);
		const auto rebuild_end = std::chrono::steady_clock::now();
		const auto upload_start = std::chrono::steady_clock::now();
		if (!UploadMeshBuffers(vertices, indices, shadow_indices)) return false;
		const auto upload_end = std::chrono::steady_clock::now();
		mesh_revision_ = world.Revision();
		mesh_valid_ = true;
