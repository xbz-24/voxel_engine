#pragma once

#include <volk.h>

#include <cstdint>
#include <span>
#include <vector>

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
		void DrawCrosshair(VkFormat format);

		VkExtent2D extent_{};
		std::vector<std::uint32_t> pixels_;
	};
}
