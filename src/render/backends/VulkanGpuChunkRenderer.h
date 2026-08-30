#pragma once

#include "VulkanGpuChunkRendererMeshOperations.h"

#include <cstdint>
#include <filesystem>

class Camera;

namespace ve::world
{
	class World;
}

namespace ve::rendering
{
	class VulkanBackend;

	/** Callback used to record overlay commands inside the GPU chunk render pass. */
	using VulkanOverlayRecordCallback = VulkanGpuOverlayRecordCallback;

	/** Renders visible voxel chunks through a Vulkan graphics pipeline. */
	class VulkanGpuChunkRenderer : protected VulkanGpuChunkRendererMeshOperations
	{
	public:
		[[nodiscard]] bool Initialize(
			VulkanBackend& backend,
			VkCommandPool command_pool,
			const std::filesystem::path& block_texture_directory,
			const std::filesystem::path& shader_directory,
			const VoxelRenderStyle& render_style,
			std::size_t frame_resource_count);

		[[nodiscard]] bool NeedsWorldMeshUpdate(const ve::world::World& world) const noexcept;
		[[nodiscard]] bool EnsureWorldMesh(
			const ve::world::World& world,
			const ve::blocks::BlockRegistry& block_registry);
		[[nodiscard]] bool Record(
			VkCommandBuffer command_buffer,
			std::uint32_t image_index,
			std::size_t frame_index,
			float elapsed_seconds,
			const Camera& camera,
			VulkanOverlayRecordCallback overlay_callback = nullptr,
			void* overlay_user_data = nullptr);

		void Release();
		[[nodiscard]] bool IsInitialized() const noexcept;
		[[nodiscard]] std::uint32_t IndexCount() const noexcept;
		[[nodiscard]] const VulkanGpuChunkMeshStats& MeshStats() const noexcept;
		[[nodiscard]] VkRenderPass RenderPass() const noexcept;
	};
}
