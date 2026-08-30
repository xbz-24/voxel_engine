#pragma once

#include "VulkanGpuChunkRendererVertexTypes.h"

#include <span>
#include <vector>

namespace ve::rendering::detail
{
	[[nodiscard]] std::vector<std::uint32_t> BuildDirectionalShadowIndices(
		std::span<const VoxelVertex> vertices,
		std::span<const std::uint32_t> indices,
		glm::vec3 sun_direction);
}
