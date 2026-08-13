#pragma once

#include "voxel/Materials.h"

#include <algorithm>

namespace voxel::detail
{
	[[nodiscard]] inline float ClampToNormalizedFloatRange(float value) noexcept
	{
		return std::clamp(value, NormalizedFloatRange.minimum, NormalizedFloatRange.maximum);
	}

	[[nodiscard]] inline float ClampToNonNegativeFloat(float value) noexcept
	{
		return std::max(0.0f, value);
	}

	[[nodiscard]] inline Color ClampColorToNormalizedRange(Color value) noexcept
	{
		return {
			ClampToNormalizedFloatRange(value.r),
			ClampToNormalizedFloatRange(value.g),
			ClampToNormalizedFloatRange(value.b),
			ClampToNormalizedFloatRange(value.a)
		};
	}
}
