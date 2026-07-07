#include "VulkanSoftwareVoxelRasterizer.h"

#include "VulkanSoftwareRasterizerBlockColor.h"
#include "VulkanSoftwareVoxelRasterizerRaycast.h"

#include <algorithm>

namespace ve::rendering
{
	void VulkanSoftwareVoxelRasterizer::RenderSamples(const RasterWork& work, std::size_t begin, std::size_t end)
	{
		for (std::size_t index = begin; index < end; ++index)
		{
			const CachedSampleRay& sample = ray_cache_[index];
			const VulkanRasterRayHit hit = TraceWorld(*work.world, work.origin, sample.direction, work.max_ray_distance);
			Rgb base_color = SkyColor(sample.direction.y);
			if (hit.hit)
			{
				const ve::blocks::BlockFace face = FaceForNormal(hit.normal);
				const glm::vec2 uv = HitUv(hit, work.origin, sample.direction);
				base_color = ApplyBlockTint(hit.block, face, UnpackRgb(TextureFor(hit.block, face).Sample(uv.x, uv.y)));
			}

			const Rgb color = hit.hit ? ShadeHit(hit, sample.direction, base_color, work.max_ray_distance, work.fog_strength) : base_color;
			const std::uint32_t packed = PackColor(color, work.format);
			if (sample.end_x == sample.begin_x + 1u && sample.end_y == sample.begin_y + 1u)
			{
				render_pixels_[(static_cast<std::size_t>(sample.begin_y) * render_extent_.width) + sample.begin_x] = packed;
				continue;
			}

			for (std::uint32_t fill_y = sample.begin_y; fill_y < sample.end_y; ++fill_y)
			{
				std::uint32_t* row = render_pixels_.data() + (static_cast<std::size_t>(fill_y) * render_extent_.width);
				std::fill(row + sample.begin_x, row + sample.end_x, packed);
			}
		}
	}
}
