#pragma once

namespace ve::gameplay
{
	inline constexpr int MinimumRenderDistanceChunks = 1;
	inline constexpr int DefaultRenderDistanceChunks = 8;
	inline constexpr int MaximumRenderDistanceChunks = 16;

	[[nodiscard]] constexpr int ClampRenderDistanceChunks(int value) noexcept
	{
		if (value < MinimumRenderDistanceChunks) return MinimumRenderDistanceChunks;
		if (value > MaximumRenderDistanceChunks) return MaximumRenderDistanceChunks;
		return value;
	}

	[[nodiscard]] constexpr float RenderDistanceSliderRatio(int value) noexcept
	{
		const int clamped = ClampRenderDistanceChunks(value);
		constexpr float range = static_cast<float>(MaximumRenderDistanceChunks - MinimumRenderDistanceChunks);
		return static_cast<float>(clamped - MinimumRenderDistanceChunks) / range;
	}
}
