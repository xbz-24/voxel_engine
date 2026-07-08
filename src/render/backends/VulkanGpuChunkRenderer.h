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

	/** Callback used to record overlay commands inside the GPU chunk render pass. */
	using VulkanOverlayRecordCallback = void (*)(VkCommandBuffer command_buffer, void* user_data);

	/** Diagnostics for the most recent GPU chunk mesh rebuild and upload. */
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

	/** Renders visible voxel chunks through a Vulkan graphics pipeline. */
	class VulkanGpuChunkRenderer
	{
	public:
		/** Creates render pass, pipeline, mesh buffers, textures, and swapchain resources. */
		[[nodiscard]] bool Initialize(VulkanBackend& backend, VkCommandPool command_pool, const std::filesystem::path& block_texture_directory, const std::filesystem::path& shader_directory);

		/** @return True when cached world geometry no longer matches the world revision. */
		[[nodiscard]] bool NeedsWorldMeshUpdate(const ve::world::World& world) const noexcept;

		/** Rebuilds and uploads world geometry when the cached mesh is stale. */
		[[nodiscard]] bool EnsureWorldMesh(const ve::world::World& world, const ve::blocks::BlockRegistry& block_registry);

		/** Records the chunk draw pass and optional overlay commands for one swapchain image. */
		[[nodiscard]] bool Record(VkCommandBuffer command_buffer, std::uint32_t image_index, const Camera& camera, VulkanOverlayRecordCallback overlay_callback = nullptr, void* overlay_user_data = nullptr);

		/** Releases all Vulkan resources owned by the renderer. */
		void Release();

		/** @return True after successful initialization and before release. */
		[[nodiscard]] bool IsInitialized() const noexcept;

		/** @return Index count uploaded for the currently cached mesh. */
		[[nodiscard]] std::uint32_t IndexCount() const noexcept;

		/** @return Last collected mesh rebuild and upload diagnostics. */
		[[nodiscard]] const VulkanGpuChunkMeshStats& MeshStats() const noexcept;

		/** @return Render pass used by frame orchestration and overlay integration. */
		[[nodiscard]] VkRenderPass RenderPass() const noexcept;

	private:
		/** CPU-side mesh cache for a world chunk and its revision. */
		struct CachedChunkMesh
		{
			int chunk_x = 0;
			int chunk_z = 0;
			std::uint64_t mesh_revision = std::numeric_limits<std::uint64_t>::max();
			std::vector<VoxelVertex> vertices;
			std::vector<std::uint32_t> indices;
		};

		/** Creates the color and depth render pass used by chunk rendering. */
		[[nodiscard]] bool CreateRenderPass();

		/** Loads shaders and creates the Vulkan pipeline state. */
		[[nodiscard]] bool CreatePipeline(const std::filesystem::path& shader_directory);

		/** Creates descriptor-free pipeline layout state for push constants. */
		[[nodiscard]] bool CreatePipelineLayout();

		/** Builds the graphics pipeline from compiled vertex and fragment modules. */
		[[nodiscard]] bool CreateGraphicsPipeline(VkShaderModule vertex_shader, VkShaderModule fragment_shader);

		/** Recreates views and framebuffers tied to the active swapchain. */
		[[nodiscard]] bool CreateSwapchainResources();

		/** Creates depth image resources matching the active swapchain extent. */
		[[nodiscard]] bool CreateDepthResources();

		/** Creates one framebuffer per swapchain image view. */
		[[nodiscard]] bool CreateFramebuffers();

		/** Uploads the flattened world mesh into device-local vertex and index buffers. */
		[[nodiscard]] bool UploadMeshBuffers(std::span<const VoxelVertex> vertices, std::span<const std::uint32_t> indices);

		/** Allocates a Vulkan buffer with caller-provided usage and memory properties. */
		[[nodiscard]] bool CreateBuffer(VkDeviceSize byte_size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer& buffer, VkDeviceMemory& memory) const;

		/** Allocates a host-visible buffer for staging or CPU-written data. */
		[[nodiscard]] bool CreateHostBuffer(VkDeviceSize byte_size, VkBufferUsageFlags usage, VkBuffer& buffer, VkDeviceMemory& memory) const;

		/** Stages data into a device-local buffer, growing capacity only when needed. */
		[[nodiscard]] bool UploadDeviceLocalBuffer(
			const void* source,
			VkDeviceSize byte_size,
			VkBufferUsageFlags usage,
			VkBuffer& buffer,
			VkDeviceMemory& memory,
			VkDeviceSize& capacity_bytes) const;

		/** Creates an image and binds device-local memory for renderer-owned resources. */
		[[nodiscard]] bool CreateDeviceImage(VkImageCreateInfo image_info, VkDeviceMemory& memory, VkImage& image) const;

		/** Copies CPU data into mapped Vulkan memory. */
		[[nodiscard]] bool CopyToDeviceBuffer(VkDeviceMemory memory, const void* source, VkDeviceSize byte_size) const;

		/** Submits a short-lived command buffer and waits for completion. */
		[[nodiscard]] bool RunImmediateCommands(void (*record)(VkCommandBuffer, void*), void* user_data) const;

		/** Loads a SPIR-V shader file and creates a Vulkan shader module. */
		[[nodiscard]] VkShaderModule CreateShaderModule(const std::filesystem::path& path) const;

		/** Releases resources that must be rebuilt when the swapchain changes. */
		void ReleaseSwapchainResources();

		/** Releases uploaded mesh buffers and resets mesh buffer capacities. */
		void ReleaseMeshBuffers();

		/** Releases block texture images, samplers, and descriptor resources. */
		void ReleaseTextureResources();

		/** Releases pipeline, pipeline layout, and render pass resources. */
		void ReleasePipelineResources();

		/** Rebuilds the flattened visible mesh for all cached world chunks. */
		void RebuildMesh(const ve::world::World& world, const ve::blocks::BlockRegistry& block_registry, std::vector<VoxelVertex>& vertices, std::vector<std::uint32_t>& indices);

		/** Clears cached chunk meshes when the world storage identity changes. */
		void ResetChunkMeshCacheForWorldStorage(const ve::world::World& world);

		/** @return Cache entry associated with a chunk coordinate pair. */
		[[nodiscard]] CachedChunkMesh& CachedMeshFor(const Chunk& chunk);

		/** Rebuilds one chunk mesh when its revision changed. */
		void RebuildChunkMesh(const ve::world::World& world, const ve::blocks::BlockRegistry& block_registry, const Chunk& chunk, CachedChunkMesh& cached_mesh) const;

		/** Appends a cached chunk mesh into the frame upload arrays. */
		void AppendCachedChunkMesh(const CachedChunkMesh& cached_mesh, std::vector<VoxelVertex>& vertices, std::vector<std::uint32_t>& indices) const;

		/** Emits only faces that are visible from neighboring block state. */
		void AppendVisibleBlockFaces(const ve::world::World& world, const ve::blocks::BlockRegistry& block_registry, int block_x, int block_y, int block_z, ve::blocks::BlockId block, std::vector<VoxelVertex>& vertices, std::vector<std::uint32_t>& indices) const;

		/** Emits vertices and indices for a single block face. */
		void AppendFaceMesh(const BlockFaceGeometry& face, const ve::world::World& world, const ve::blocks::BlockRegistry& block_registry, int block_x, int block_y, int block_z, ve::blocks::BlockId block, std::vector<VoxelVertex>& vertices, std::vector<std::uint32_t>& indices) const;

		/** @return Memory type index matching the requested Vulkan memory properties. */
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
