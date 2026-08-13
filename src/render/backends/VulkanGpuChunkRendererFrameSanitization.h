#pragma once

#include <algorithm>
#include <cmath>
#include <glm/glm.hpp>

namespace ve::rendering::detail
{
	[[nodiscard]] inline float FiniteNonNegative(float value, float fallback) noexcept
	{
		return std::isfinite(value) && value >= 0.0f ? value : fallback;
	}

	[[nodiscard]] inline float FiniteInRange(float value, float fallback, float maximum) noexcept
	{
		return std::isfinite(value) ? std::clamp(value, 0.0f, maximum) : fallback;
	}

	[[nodiscard]] inline glm::vec3 FiniteNonNegativeColor(
		glm::vec3 color,
		glm::vec3 fallback) noexcept
	{
		return {
			FiniteNonNegative(color.x, fallback.x),
			FiniteNonNegative(color.y, fallback.y),
			FiniteNonNegative(color.z, fallback.z)
		};
	}

	[[nodiscard]] inline glm::vec3 FinitePosition(glm::vec3 position) noexcept
	{
		position.x = std::isfinite(position.x) ? position.x : 0.0f;
		position.y = std::isfinite(position.y) ? position.y : 0.0f;
		position.z = std::isfinite(position.z) ? position.z : 0.0f;
		return position;
	}
}
