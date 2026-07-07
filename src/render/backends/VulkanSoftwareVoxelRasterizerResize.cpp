#include "VulkanSoftwareVoxelRasterizer.h"

#include <algorithm>
#include <cstdint>

namespace ve::rendering
{
	namespace
	{
		[[nodiscard]] VkExtent2D InternalRenderExtentFor(VkExtent2D output_extent, const VulkanDemoSettings& settings) noexcept
		{
			const std::uint32_t max_width = std::clamp(settings.max_internal_width, 320u, 1920u);
			const std::uint32_t max_height = std::clamp(settings.max_internal_height, 180u, 1080u);
			if (output_extent.width <= max_width && output_extent.height <= max_height) return output_extent;

			const std::uint64_t width_limited_height = static_cast<std::uint64_t>(output_extent.height) * max_width;
			const std::uint64_t height_limited_width = static_cast<std::uint64_t>(output_extent.width) * max_height;
			if (width_limited_height <= height_limited_width)
			{
				return { max_width, std::max(1u, static_cast<std::uint32_t>(width_limited_height / output_extent.width)) };
			}
			return { std::max(1u, static_cast<std::uint32_t>(height_limited_width / output_extent.height)), max_height };
		}
	}

	bool VulkanSoftwareVoxelRasterizer::Resize(VkExtent2D extent, const VulkanDemoSettings& settings)
	{
		if (extent.width == 0 || extent.height == 0) return false;

		const VkExtent2D next_render_extent = InternalRenderExtentFor(extent, settings);
		if (extent_.width == extent.width && extent_.height == extent.height &&
			render_extent_.width == next_render_extent.width && render_extent_.height == next_render_extent.height)
		{
			return true;
		}

		extent_ = extent;
		render_extent_ = next_render_extent;
		pixels_.assign(static_cast<std::size_t>(extent.width) * static_cast<std::size_t>(extent.height), 0u);
		render_pixels_.assign(static_cast<std::size_t>(render_extent_.width) * static_cast<std::size_t>(render_extent_.height), 0u);
		RebuildUpscaleLookup();
		ray_cache_valid_ = false;
		return true;
	}
}
