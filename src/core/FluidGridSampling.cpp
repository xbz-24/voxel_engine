#include "FluidGrid.h"

#include <algorithm>
#include <cmath>

namespace ve::simulation
{
	namespace
	{
		/** @param value Floating point coordinate. @param maximum Largest valid coordinate. @return Clamped coordinate. */
		float ClampCoordinate(float value, int maximum) noexcept
		{
			return std::clamp(value, 0.0f, ve::core::ToFloat(maximum));
		}
	}

	/** Samples density with bilinear interpolation. */
	float FluidGrid::SampleDensity(const glm::vec2& position) const noexcept
	{
		const float sample_x = ClampCoordinate(position.x, width_ - 1);
		const float sample_y = ClampCoordinate(position.y, height_ - 1);
		const int left_cell_x = ve::core::ToInt(sample_x);
		const int lower_cell_y = ve::core::ToInt(sample_y);
		const int right_cell_x = left_cell_x + 1;
		const int upper_cell_y = lower_cell_y + 1;
		const float horizontal_weight = sample_x - ve::core::ToFloat(left_cell_x);
		const float vertical_weight = sample_y - ve::core::ToFloat(lower_cell_y);
		const float lower_density = std::lerp(
			DensityAt(left_cell_x, lower_cell_y),
			DensityAt(right_cell_x, lower_cell_y),
			horizontal_weight);
		const float upper_density = std::lerp(
			DensityAt(left_cell_x, upper_cell_y),
			DensityAt(right_cell_x, upper_cell_y),
			horizontal_weight);
		return std::lerp(lower_density, upper_density, vertical_weight);
	}

	/** Samples velocity with bilinear interpolation. */
	glm::vec2 FluidGrid::SampleVelocity(const glm::vec2& position) const noexcept
	{
		const float sample_x = ClampCoordinate(position.x, width_ - 1);
		const float sample_y = ClampCoordinate(position.y, height_ - 1);
		const int left_cell_x = ve::core::ToInt(sample_x);
		const int lower_cell_y = ve::core::ToInt(sample_y);
		const int right_cell_x = left_cell_x + 1;
		const int upper_cell_y = lower_cell_y + 1;
		const float horizontal_weight = sample_x - ve::core::ToFloat(left_cell_x);
		const float vertical_weight = sample_y - ve::core::ToFloat(lower_cell_y);
		const glm::vec2 lower_velocity = glm::mix(
			VelocityAt(left_cell_x, lower_cell_y),
			VelocityAt(right_cell_x, lower_cell_y),
			horizontal_weight);
		const glm::vec2 upper_velocity = glm::mix(
			VelocityAt(left_cell_x, upper_cell_y),
			VelocityAt(right_cell_x, upper_cell_y),
			horizontal_weight);
		return glm::mix(lower_velocity, upper_velocity, vertical_weight);
	}
}
