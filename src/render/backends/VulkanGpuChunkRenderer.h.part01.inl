
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
		std::uint32_t last_shadow_index_count = 0;
		std::uint32_t last_rebuilt_chunk_count = 0;
		std::uint32_t cached_chunk_count = 0;
		VkDeviceSize vertex_buffer_capacity_bytes = 0;
		VkDeviceSize index_buffer_capacity_bytes = 0;
		VkDeviceSize shadow_index_buffer_capacity_bytes = 0;
	};

	/** Renders visible voxel chunks through a Vulkan graphics pipeline. */
	class VulkanGpuChunkRenderer
	{
	public:
		/** Creates render pass, pipeline, mesh buffers, textures, and swapchain resources. */
		[[nodiscard]] bool Initialize(VulkanBackend& backend,
			VkCommandPool command_pool,
			const std::filesystem::path& block_texture_directory,
			const std::filesystem::path& shader_directory,
			const VoxelRenderStyle& render_style,
			std::size_t frame_resource_count);

		/** @return True when cached world geometry no longer matches the world revision. */
		[[nodiscard]] bool NeedsWorldMeshUpdate(const ve::world::World& world) const noexcept;

		/** Rebuilds and uploads world geometry when the cached mesh is stale. */
		[[nodiscard]] bool EnsureWorldMesh(const ve::world::World& world, const ve::blocks::BlockRegistry& block_registry);

		/** Records the chunk draw pass and optional overlay commands for one swapchain image. */
		[[nodiscard]] bool Record(
			VkCommandBuffer command_buffer,
			std::uint32_t image_index,
			std::size_t frame_index,
			float elapsed_seconds,
			const Camera& camera,
			VulkanOverlayRecordCallback overlay_callback = nullptr,
			void* overlay_user_data = nullptr);

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

