#pragma once

namespace voxel
{
	/** Inclusive numeric range helper used by material validation. */
	struct FloatRange
	{
		float minimum = 0.0f;
		float maximum = 1.0f;

		[[nodiscard]] constexpr bool Contains(float value) const noexcept
		{
			return value >= minimum && value <= maximum;
		}
	};

	inline constexpr FloatRange NormalizedFloatRange{ 0.0f, 1.0f };
	inline constexpr float MinimumEmissionStrength = 0.0f;

	/** Linear RGBA color with normalized channels. */
	struct Color
	{
		float r = 1.0f;
		float g = 1.0f;
		float b = 1.0f;
		float a = 1.0f;
	};
}
