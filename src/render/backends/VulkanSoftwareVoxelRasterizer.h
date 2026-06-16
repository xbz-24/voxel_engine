#pragma once

#include "Block.h"

#include <volk.h>

#include <array>
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
	struct VulkanFrameTiming
	{
		double snapshot_cpu_ms = 0.0;
		double raster_cpu_ms = 0.0;
		double upscale_cpu_ms = 0.0;
		double upload_cpu_ms = 0.0;
		double present_cpu_ms = 0.0;
		double gpu_copy_ms = 0.0;
		bool has_gpu_copy_timing = false;
		std::uint32_t worker_count = 1;
		std::uint32_t sample_step = 1;
		VkExtent2D render_extent{};
	};

	struct VulkanDemoSettings
	{
		std::uint32_t max_internal_width = 1280;
		std::uint32_t max_internal_height = 720;
		std::uint32_t pixel_block_size = 1;
		float max_ray_distance = 72.0f;
		float fog_strength = 0.45f;
		float outline_strength = 0.12f;
		bool adaptive_quality = false;
		bool show_tuning_panel = true;
		bool show_debug_overlay = true;
	};

	struct VulkanDemoInput
	{
		double mouse_x = 0.0;
		double mouse_y = 0.0;
		bool mouse_left_down = false;
		bool mouse_left_pressed = false;
	};

	struct VulkanSoftwareVoxelRasterizerFrame
	{
		VulkanSoftwareVoxelRasterizerFrame(const ve::world::World& frameWorld,
			const Camera& frameCamera,
			VkExtent2D frameExtent,
			VkFormat frameFormat,
			int frameDisplayedFps,
			double frameDeltaSeconds,
			VulkanFrameTiming frameTiming,
			const VulkanDemoInput& frameInput,
			VulkanDemoSettings& frameSettings) noexcept
			: world(frameWorld),
			  camera(frameCamera),
			  extent(frameExtent),
			  format(frameFormat),
			  displayed_fps(frameDisplayedFps),
			  delta_seconds(frameDeltaSeconds),
			  previous_timing(frameTiming),
			  input(frameInput),
			  settings(frameSettings)
		{
		}

		const ve::world::World& world;
		const Camera& camera;
		VkExtent2D extent{};
		VkFormat format = VK_FORMAT_UNDEFINED;
		int displayed_fps = 0;
		double delta_seconds = 0.0;
		VulkanFrameTiming previous_timing{};
		const VulkanDemoInput& input;
		VulkanDemoSettings& settings;
	};

	/** Temporary Vulkan render strategy that rasterizes voxel visibility on the CPU. */
	class VulkanSoftwareVoxelRasterizer
	{
	public:
		struct FrameWorldSnapshot
		{
			std::vector<ve::blocks::BlockId> blocks;
			int width = 0;
			int height = 0;
			int depth = 0;
			std::uint64_t world_revision = 0;

			[[nodiscard]] ve::blocks::BlockId GetBlock(const glm::ivec3& position) const noexcept;
		};

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
		struct CachedSampleRay
		{
			std::uint32_t x = 0;
			std::uint32_t y = 0;
			std::uint32_t x_end = 0;
			std::uint32_t y_end = 0;
			glm::vec3 direction{ 0.0f, 0.0f, -1.0f };
		};

		struct CpuTexture
		{
			std::vector<std::uint32_t> pixels;
			std::uint32_t width = 1;
			std::uint32_t height = 1;

			[[nodiscard]] std::uint32_t Sample(float u, float v) const noexcept;
		};

		struct TextureLibrary
		{
			std::vector<CpuTexture> textures;
			std::array<std::uint16_t, static_cast<std::size_t>(ve::blocks::BlockId::Count) * static_cast<std::size_t>(ve::blocks::BlockFace::Count)> face_texture_indices{};
			bool loaded = false;
		};

		struct RasterWork
		{
			const FrameWorldSnapshot* world = nullptr;
			glm::vec3 origin{ 0.0f };
			VkFormat format = VK_FORMAT_UNDEFINED;
			float max_ray_distance = 128.0f;
			float fog_strength = 0.45f;
		};

		struct UpscaleRange
		{
			std::uint32_t begin = 0;
			std::uint32_t end = 0;
		};

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
		void DrawText(const char* text, std::uint32_t x, std::uint32_t y, std::uint32_t scale, std::uint32_t color);
		void DrawFilledRect(std::uint32_t x, std::uint32_t y, std::uint32_t width, std::uint32_t height, std::uint32_t color);
		bool Slider(const VulkanSoftwareVoxelRasterizerFrame& frame,
			const char* label,
			float& value,
			float min_value,
			float max_value,
			std::uint32_t x,
			std::uint32_t y,
			std::uint32_t width);

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
