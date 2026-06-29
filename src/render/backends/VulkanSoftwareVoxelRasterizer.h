#pragma once

#include "VulkanDemoFrameTypes.h"
#include "VulkanSoftwareVoxelRasterizerData.h"

#include <volk.h>

#include <atomic>
#include <condition_variable>
#include <cstdint>
#include <filesystem>
#include <mutex>
#include <span>
#include <thread>
#include <vector>

#include <glm/glm.hpp>

class Camera;

namespace ve::world
{
	class World;
}

namespace ve::rendering
{
	/** Temporary Vulkan render strategy that rasterizes voxel visibility on the CPU. */
	class VulkanSoftwareVoxelRasterizer
	{
	public:
		using FrameWorldSnapshot = VulkanRasterFrameWorldSnapshot;

		~VulkanSoftwareVoxelRasterizer();

		void LoadBlockTextures(const std::filesystem::path& block_texture_directory);
		[[nodiscard]] bool Resize(VkExtent2D extent, const VulkanDemoSettings& settings);
		void Render(const VulkanSoftwareVoxelRasterizerFrame& frame);
		void Release();

		[[nodiscard]] std::span<const std::uint32_t> Pixels() const noexcept;
		[[nodiscard]] VkExtent2D Extent() const noexcept;
		[[nodiscard]] std::span<const std::uint32_t> RenderPixels() const noexcept;
		[[nodiscard]] VkExtent2D RenderExtent() const noexcept;
		[[nodiscard]] const VulkanFrameTiming& LastTiming() const noexcept;

	private:
		using CachedSampleRay = VulkanRasterCachedSampleRay;
		using CpuTexture = VulkanRasterCpuTexture;
		using TextureLibrary = VulkanRasterTextureLibrary;
		using RasterWork = VulkanRasterWork;
		using UpscaleRange = VulkanRasterUpscaleRange;

		void CaptureWorldSnapshot(const ve::world::World& world);
		[[nodiscard]] const CpuTexture& TextureFor(ve::blocks::BlockId block, ve::blocks::BlockFace face) const noexcept;
		void EnsureWorkers();
		void EnsureRayCache(const Camera& camera, std::uint32_t sample_step);
		void RenderSamples(const RasterWork& work, std::size_t begin, std::size_t end);
		void RenderSamplesMultithreaded(const RasterWork& work);
		void RenderWorkerLoop(std::uint64_t initial_generation);
		void CompleteWorkerBatch();
		void RebuildUpscaleLookup();
		void UpscaleRenderPixels();
		void ApplyVoxelOutlines(VkFormat format, float strength);
		void DrawCrosshair(VkFormat format);
		void DrawDemoOverlay(const VulkanSoftwareVoxelRasterizerFrame& frame);
		void DrawTuningPanel(const VulkanSoftwareVoxelRasterizerFrame& frame);
		void DrawText(
			const char* text,
			std::uint32_t origin_x,
			std::uint32_t origin_y,
			std::uint32_t scale,
			std::uint32_t color);
		void DrawFilledRect(
			std::uint32_t origin_x,
			std::uint32_t origin_y,
			std::uint32_t width,
			std::uint32_t height,
			std::uint32_t color);
		bool Slider(const VulkanSoftwareVoxelRasterizerFrame& frame,
			const char* label,
			float& value,
			float min_value,
			float max_value,
			std::uint32_t origin_x,
			std::uint32_t origin_y,
			std::uint32_t control_width);

		VkExtent2D extent_{};
		VkExtent2D render_extent_{};
		std::vector<std::uint32_t> pixels_;
		std::vector<std::uint32_t> render_pixels_;
		std::vector<std::uint32_t> outline_pixels_;
		std::vector<UpscaleRange> upscale_x_ranges_;
		std::vector<UpscaleRange> upscale_y_ranges_;
		std::vector<CachedSampleRay> ray_cache_;
		FrameWorldSnapshot world_snapshot_;
		glm::vec3 cached_forward_{ 0.0f, 0.0f, -1.0f };
		glm::vec3 cached_right_{ 1.0f, 0.0f, 0.0f };
		glm::vec3 cached_up_{ 0.0f, 1.0f, 0.0f };
		std::uint32_t cached_sample_step_ = 0;
		bool ray_cache_valid_ = false;
		VulkanFrameTiming last_timing_{};
		TextureLibrary texture_library_;
		std::vector<std::jthread> workers_;
		std::mutex work_mutex_;
		std::condition_variable work_available_;
		std::condition_variable work_complete_;
		RasterWork current_work_{};
		std::atomic_size_t next_sample_index_{ 0 };
		std::uint64_t work_generation_ = 0;
		std::size_t active_workers_ = 0;
		bool stop_workers_ = false;
	};
}
