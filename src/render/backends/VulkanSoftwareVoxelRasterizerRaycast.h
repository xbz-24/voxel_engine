#pragma once

#include "VulkanSoftwareVoxelRasterizerData.h"
#include "VulkanSoftwareRasterizerColor.h"

#include <glm/glm.hpp>

namespace ve::rendering
{
	struct VulkanRasterRayHit
	{
		bool hit = false;
		ve::blocks::BlockId block = ve::blocks::BlockId::Air;
		glm::ivec3 position{ 0 };
		glm::ivec3 normal{ 0, 1, 0 };
		float distance = 0.0f;
	};

	[[nodiscard]] glm::vec3 FindAirCameraOrigin(const VulkanRasterFrameWorldSnapshot& world, glm::vec3 origin);
	[[nodiscard]] VulkanRasterRayHit TraceWorld(const VulkanRasterFrameWorldSnapshot& world,
		const glm::vec3& origin,
		const glm::vec3& direction,
		float max_ray_distance);
	[[nodiscard]] ve::blocks::BlockFace FaceForNormal(const glm::ivec3& normal) noexcept;
	[[nodiscard]] glm::vec2 HitUv(const VulkanRasterRayHit& hit, const glm::vec3& origin, const glm::vec3& direction) noexcept;
	[[nodiscard]] Rgb ShadeHit(const VulkanRasterRayHit& hit,
		const glm::vec3& direction,
		Rgb base_color,
		float max_ray_distance,
		float fog_strength) noexcept;
}
