#include "VulkanSoftwareVoxelRasterizerRaycast.h"

#include <algorithm>
#include <cmath>

namespace ve::rendering
{
	ve::blocks::BlockFace FaceForNormal(const glm::ivec3& normal) noexcept
	{
		if (normal.y > 0) return ve::blocks::BlockFace::Top;
		if (normal.y < 0) return ve::blocks::BlockFace::Bottom;
		if (normal.z > 0) return ve::blocks::BlockFace::Front;
		if (normal.z < 0) return ve::blocks::BlockFace::Back;
		if (normal.x > 0) return ve::blocks::BlockFace::Right;
		return ve::blocks::BlockFace::Left;
	}

	glm::vec2 HitUv(const VulkanRasterRayHit& hit, const glm::vec3& origin, const glm::vec3& direction) noexcept
	{
		const glm::vec3 point = origin + (direction * std::max(hit.distance, 0.0f));
		const auto fract = [](float value)
		{
			const float fractional_part = value - std::floor(value);
			return std::clamp(fractional_part, 0.0f, 0.9999f);
		};
		if (hit.normal.y != 0) return { fract(point.x), fract(point.z) };
		if (hit.normal.x != 0) return { fract(point.z), fract(point.y) };
		return { fract(point.x), fract(point.y) };
	}

	Rgb ShadeHit(const VulkanRasterRayHit& hit, const glm::vec3& direction, Rgb base_color, float max_ray_distance, float fog_strength) noexcept
	{
		float shade = 0.72f;
		if (hit.normal.y > 0) shade = 1.05f;
		else if (hit.normal.y < 0) shade = 0.45f;
		else if (hit.normal.x != 0) shade = 0.78f;

		Rgb color = { Scale(base_color.r, shade), Scale(base_color.g, shade), Scale(base_color.b, shade) };
		const float fog = std::clamp(hit.distance / std::max(max_ray_distance, 1.0f), 0.0f, 1.0f);
		return Mix(color, SkyColor(direction.y), fog * std::clamp(fog_strength, 0.0f, 1.0f));
	}
}
