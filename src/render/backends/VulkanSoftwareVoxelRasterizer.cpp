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

	std::uint32_t VulkanRasterCpuTexture::Sample(float u, float v) const noexcept
	{
		if (pixels.empty()) return PackRgb({ 132, 132, 132 });

		const float wrapped_u = u - std::floor(u);
		const float wrapped_v = v - std::floor(v);
		const auto x = static_cast<std::uint32_t>(std::clamp(wrapped_u, 0.0f, 0.9999f) * static_cast<float>(width));
		const auto y = static_cast<std::uint32_t>(std::clamp(1.0f - wrapped_v, 0.0f, 0.9999f) * static_cast<float>(height));
		const std::size_t index = static_cast<std::size_t>(std::min(y, height - 1u)) * width + std::min(x, width - 1u);
		return pixels[index];
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
