#pragma once

#include <volk.h>

#include <cstdint>
#include <span>
#include <vector>

#include <glm/glm.hpp>

class Camera;

namespace ve::world
{
	class World;
}

namespace ve::rendering
{
	struct VulkanSoftwareVoxelRasterizerFrame
	{
		VulkanSoftwareVoxelRasterizerFrame(const ve::world::World& frameWorld,
			const Camera& frameCamera,
			VkExtent2D frameExtent,
			VkFormat frameFormat) noexcept
			: world(frameWorld),
			  camera(frameCamera),
			  extent(frameExtent),
			  format(frameFormat)
		{
		}

		const ve::world::World& world;
		const Camera& camera;
		VkExtent2D extent{};
		VkFormat format = VK_FORMAT_UNDEFINED;
	};

	/** Temporary Vulkan render strategy that rasterizes voxel visibility on the CPU. */
	class VulkanSoftwareVoxelRasterizer
	{
	public:
		[[nodiscard]] bool Resize(VkExtent2D extent);
		void Render(const VulkanSoftwareVoxelRasterizerFrame& frame);

		[[nodiscard]] std::span<const std::uint32_t> Pixels() const noexcept;
		[[nodiscard]] VkExtent2D Extent() const noexcept;

	private:
		struct CachedSampleRay
		{
			std::uint32_t x = 0;
			std::uint32_t y = 0;
			std::uint32_t x_end = 0;
			std::uint32_t y_end = 0;
			glm::vec3 direction{ 0.0f, 0.0f, -1.0f };
		};

		void EnsureRayCache(const Camera& camera, std::uint32_t sample_step);
		void DrawCrosshair(VkFormat format);
		void DrawDemoOverlay(VkFormat format);

		VkExtent2D extent_{};
		std::vector<std::uint32_t> pixels_;
		std::vector<CachedSampleRay> ray_cache_;
		glm::vec3 cached_forward_{ 0.0f, 0.0f, -1.0f };
		glm::vec3 cached_right_{ 1.0f, 0.0f, 0.0f };
		glm::vec3 cached_up_{ 0.0f, 1.0f, 0.0f };
		std::uint32_t cached_sample_step_ = 0;
		bool ray_cache_valid_ = false;
	};
}
