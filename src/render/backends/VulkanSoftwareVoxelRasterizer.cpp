#include "VulkanSoftwareVoxelRasterizer.h"

#include "VulkanSoftwareRasterizerColor.h"

#include <algorithm>
#include <cmath>

namespace ve::rendering
{
	VulkanSoftwareVoxelRasterizer::~VulkanSoftwareVoxelRasterizer()
	{
		Release();
	}

	ve::blocks::BlockId VulkanRasterFrameWorldSnapshot::GetBlock(const glm::ivec3& position) const noexcept
	{
		if (position.x < 0 || position.y < 0 || position.z < 0 ||
			position.x >= width || position.y >= height || position.z >= depth)
		{
			return ve::blocks::BlockId::Air;
		}

		const std::size_t index =
			((static_cast<std::size_t>(position.x) * static_cast<std::size_t>(height)) +
				static_cast<std::size_t>(position.y)) *
			static_cast<std::size_t>(depth) + static_cast<std::size_t>(position.z);
		return blocks[index];
	}

	std::uint32_t VulkanRasterCpuTexture::Sample(float texture_u, float texture_v) const noexcept
	{
		if (pixels.empty()) return PackRgb({ 132, 132, 132 });

		const float wrapped_u = texture_u - std::floor(texture_u);
		const float wrapped_v = texture_v - std::floor(texture_v);
		const auto sample_x = static_cast<std::uint32_t>(
			std::clamp(wrapped_u, 0.0f, 0.9999f) * static_cast<float>(width));
		const auto sample_y = static_cast<std::uint32_t>(
			std::clamp(1.0f - wrapped_v, 0.0f, 0.9999f) * static_cast<float>(height));
		const std::size_t pixel_index =
			static_cast<std::size_t>(std::min(sample_y, height - 1u)) * width +
			std::min(sample_x, width - 1u);
		return pixels[pixel_index];
	}

	std::span<const std::uint32_t> VulkanSoftwareVoxelRasterizer::Pixels() const noexcept
	{
		return pixels_;
	}

	VkExtent2D VulkanSoftwareVoxelRasterizer::Extent() const noexcept
	{
		return extent_;
	}

	std::span<const std::uint32_t> VulkanSoftwareVoxelRasterizer::RenderPixels() const noexcept
	{
		return render_pixels_;
	}

	VkExtent2D VulkanSoftwareVoxelRasterizer::RenderExtent() const noexcept
	{
		return render_extent_;
	}

	const VulkanFrameTiming& VulkanSoftwareVoxelRasterizer::LastTiming() const noexcept
	{
		return last_timing_;
	}
}
