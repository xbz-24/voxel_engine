#pragma once

#include "Block.h"
#include "CoreTypes.h"
#include "VulkanGpuChunkGeometry.h"
#include "VulkanGpuChunkRendererTypes.h"

#include <volk.h>

#include <array>
#include <cstdint>
#include <filesystem>
#include <limits>
#include <span>
#include <vector>

class Camera;
class Chunk;

namespace ve::world
{
	class World;
}

namespace ve::rendering
{
	class VulkanBackend;
	using VulkanOverlayRecordCallback = void (*)(VkCommandBuffer command_buffer, void* user_data);

	struct VulkanGpuChunkMeshStats
	{
		double last_rebuild_cpu_ms = 0.0;
		double last_upload_cpu_ms = 0.0;
		std::uint32_t last_vertex_count = 0;
		std::uint32_t last_index_count = 0;
		std::uint32_t last_rebuilt_chunk_count = 0;
		std::uint32_t cached_chunk_count = 0;
		VkDeviceSize vertex_buffer_capacity_bytes = 0;
		VkDeviceSize index_buffer_capacity_bytes = 0;
	};

	class VulkanGpuChunkRenderer
	{
	public:
		[[nodiscard]] bool Initialize(VulkanBackend& backend, VkCommandPool command_pool, const std::filesystem::path& block_texture_directory, const std::filesystem::path& shader_directory);
		[[nodiscard]] bool NeedsWorldMeshUpdate(const ve::world::World& world) const noexcept;
		[[nodiscard]] bool EnsureWorldMesh(const ve::world::World& world);
		[[nodiscard]] bool Record(VkCommandBuffer command_buffer, std::uint32_t image_index, const Camera& camera, VulkanOverlayRecordCallback overlay_callback = nullptr, void* overlay_user_data = nullptr);
		void Release();

		[[nodiscard]] bool IsInitialized() const noexcept;
		[[nodiscard]] std::uint32_t IndexCount() const noexcept;
		[[nodiscard]] const VulkanGpuChunkMeshStats& MeshStats() const noexcept;
		[[nodiscard]] VkRenderPass RenderPass() const noexcept;

	private:
		struct CachedChunkMesh
		{
			int chunk_x = 0;
			int chunk_z = 0;
			std::uint64_t mesh_revision = std::numeric_limits<std::uint64_t>::max();
			std::vector<VoxelVertex> vertices;
			std::vector<std::uint32_t> indices;
		};

		[[nodiscard]] bool CreateRenderPass();
		[[nodiscard]] bool CreatePipeline(const std::filesystem::path& shader_directory);
		[[nodiscard]] bool CreatePipelineLayout();
		[[nodiscard]] bool CreateGraphicsPipeline(VkShaderModule vertex_shader, VkShaderModule fragment_shader);
		[[nodiscard]] bool CreateSwapchainResources();
		[[nodiscard]] bool CreateDepthResources();
		[[nodiscard]] bool CreateFramebuffers();
		[[nodiscard]] bool UploadMeshBuffers(std::span<const VoxelVertex> vertices, std::span<const std::uint32_t> indices);
		[[nodiscard]] bool CreateBuffer(VkDeviceSize byte_size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer& buffer, VkDeviceMemory& memory) const;
		[[nodiscard]] bool CreateHostBuffer(VkDeviceSize byte_size, VkBufferUsageFlags usage, VkBuffer& buffer, VkDeviceMemory& memory) const;
		[[nodiscard]] bool UploadDeviceLocalBuffer(
			const void* source,
			VkDeviceSize byte_size,
			VkBufferUsageFlags usage,
			VkBuffer& buffer,
			VkDeviceMemory& memory,
			VkDeviceSize& capacity_bytes) const;
		[[nodiscard]] bool CreateDeviceImage(VkImageCreateInfo image_info, VkDeviceMemory& memory, VkImage& image) const;
		[[nodiscard]] bool CopyToDeviceBuffer(VkDeviceMemory memory, const void* source, VkDeviceSize byte_size) const;
		[[nodiscard]] bool RunImmediateCommands(void (*record)(VkCommandBuffer, void*), void* user_data) const;
		[[nodiscard]] VkShaderModule CreateShaderModule(const std::filesystem::path& path) const;
		void ReleaseSwapchainResources();
		void ReleaseMeshBuffers();
		void ReleaseTextureResources();
		void ReleasePipelineResources();
		void RebuildMesh(const ve::world::World& world, std::vector<VoxelVertex>& vertices, std::vector<std::uint32_t>& indices);
		void ResetChunkMeshCacheForWorldStorage(const ve::world::World& world);
		[[nodiscard]] CachedChunkMesh& CachedMeshFor(const Chunk& chunk);
		void RebuildChunkMesh(const ve::world::World& world, const Chunk& chunk, CachedChunkMesh& cached_mesh) const;
		void AppendCachedChunkMesh(const CachedChunkMesh& cached_mesh, std::vector<VoxelVertex>& vertices, std::vector<std::uint32_t>& indices) const;
		void AppendVisibleBlockFaces(const ve::world::World& world, int block_x, int block_y, int block_z, ve::blocks::BlockId block, std::vector<VoxelVertex>& vertices, std::vector<std::uint32_t>& indices) const;
		void AppendFaceMesh(const BlockFaceGeometry& face, const ve::world::World& world, int block_x, int block_y, int block_z, ve::blocks::BlockId block, std::vector<VoxelVertex>& vertices, std::vector<std::uint32_t>& indices) const;
		[[nodiscard]] static std::uint32_t FindMemoryType(VkPhysicalDevice physical_device, std::uint32_t type_filter, VkMemoryPropertyFlags properties);

		VulkanBackend* backend_ = nullptr;
		VkDevice device_ = VK_NULL_HANDLE;
		VkPhysicalDevice physical_device_ = VK_NULL_HANDLE;
		VkCommandPool command_pool_ = VK_NULL_HANDLE;
		VkRenderPass render_pass_ = VK_NULL_HANDLE;
		VkPipelineLayout pipeline_layout_ = VK_NULL_HANDLE;
		VkPipeline pipeline_ = VK_NULL_HANDLE;
		VkImage depth_image_ = VK_NULL_HANDLE;
		VkDeviceMemory depth_memory_ = VK_NULL_HANDLE;
		VkImageView depth_view_ = VK_NULL_HANDLE;
		VkBuffer vertex_buffer_ = VK_NULL_HANDLE;
		VkDeviceMemory vertex_memory_ = VK_NULL_HANDLE;
		VkDeviceSize vertex_buffer_capacity_bytes_ = 0;
		VkBuffer index_buffer_ = VK_NULL_HANDLE;
		VkDeviceMemory index_memory_ = VK_NULL_HANDLE;
		VkDeviceSize index_buffer_capacity_bytes_ = 0;
		ve::core::DynamicArray<VkImageView> swapchain_image_views_;
		ve::core::DynamicArray<VkFramebuffer> framebuffers_;
		VkExtent2D extent_{};
		VkFormat color_format_ = VK_FORMAT_UNDEFINED;
		VkFormat depth_format_ = VK_FORMAT_D32_SFLOAT;
		std::uint64_t mesh_revision_ = 0;
		std::uint64_t cached_chunk_storage_revision_ = 0;
		std::uint32_t index_count_ = 0;
		std::uint32_t last_rebuilt_chunk_count_ = 0;
		VulkanGpuChunkMeshStats mesh_stats_;
		std::vector<CachedChunkMesh> cached_chunk_meshes_;
		bool mesh_valid_ = false;
		bool initialized_ = false;
	};
}
