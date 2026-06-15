#pragma once

#include <array>

namespace ve::math
{
	struct SimdFloat4
	{
		std::array<float, 4> lanes{ { 0.0f, 0.0f, 0.0f, 0.0f } };
	};

	/** SSE-backed operations over four packed floats. */
	class SimdFloat4Math
	{
	public:
		/** @param left First vector. @param right Second vector. @return Lane-wise sum. */
		[[nodiscard]] static SimdFloat4 Add(const SimdFloat4& left, const SimdFloat4& right) noexcept;

		/** @param value Base vector. @param scale Scale vector. @param addend Added vector. @return value * scale + addend. */
		[[nodiscard]] static SimdFloat4 MultiplyAdd(const SimdFloat4& value, const SimdFloat4& scale, const SimdFloat4& addend) noexcept;

		/** @param left First vector. @param right Second vector. @return Dot product of all four lanes. */
		[[nodiscard]] static float Dot(const SimdFloat4& left, const SimdFloat4& right) noexcept;
	};
}
