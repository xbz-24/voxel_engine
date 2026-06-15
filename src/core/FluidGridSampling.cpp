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
			return std::clamp(value, 0.0f, static_cast<float>(maximum));
		}
	}

	/** Samples density with bilinear interpolation. */
	float FluidGrid::SampleDensity(const glm::vec2& position) const noexcept
	{
		const float x = ClampCoordinate(position.x, width_ - 1);
		const float y = ClampCoordinate(position.y, height_ - 1);
		const int x0 = static_cast<int>(x);
		const int y0 = static_cast<int>(y);
		const int x1 = x0 + 1;
		const int y1 = y0 + 1;
		const float tx = x - static_cast<float>(x0);
		const float ty = y - static_cast<float>(y0);
		const float bottom = std::lerp(DensityAt(x0, y0), DensityAt(x1, y0), tx);
		const float top = std::lerp(DensityAt(x0, y1), DensityAt(x1, y1), tx);
		return std::lerp(bottom, top, ty);
	}

	/** Samples velocity with bilinear interpolation. */
	glm::vec2 FluidGrid::SampleVelocity(const glm::vec2& position) const noexcept
	{
		const float x = ClampCoordinate(position.x, width_ - 1);
		const float y = ClampCoordinate(position.y, height_ - 1);
		const int x0 = static_cast<int>(x);
		const int y0 = static_cast<int>(y);
		const int x1 = x0 + 1;
		const int y1 = y0 + 1;
		const float tx = x - static_cast<float>(x0);
		const float ty = y - static_cast<float>(y0);
		const glm::vec2 bottom = glm::mix(VelocityAt(x0, y0), VelocityAt(x1, y0), tx);
		const glm::vec2 top = glm::mix(VelocityAt(x0, y1), VelocityAt(x1, y1), tx);
		return glm::mix(bottom, top, ty);
	}
}
