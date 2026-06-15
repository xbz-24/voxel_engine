#include "SimdFloat4.h"

#include <immintrin.h>

namespace ve::math
{
	namespace
	{
		/** @param value Packed float container. @return SSE register loaded from lanes. */
		__m128 Load(const SimdFloat4& value) noexcept
		{
			return _mm_loadu_ps(value.lanes.data());
		}

		/** @param value SSE register. @return Packed float container. */
		SimdFloat4 Store(__m128 value) noexcept
		{
			SimdFloat4 result;
			_mm_storeu_ps(result.lanes.data(), value);
			return result;
		}
	}

	/** Adds four packed float lanes with SSE. */
	SimdFloat4 SimdFloat4Math::Add(const SimdFloat4& left, const SimdFloat4& right) noexcept
	{
		return Store(_mm_add_ps(Load(left), Load(right)));
	}

	/** Computes value * scale + addend using SSE lanes. */
	SimdFloat4 SimdFloat4Math::MultiplyAdd(const SimdFloat4& value, const SimdFloat4& scale, const SimdFloat4& addend) noexcept
	{
		return Store(_mm_add_ps(_mm_mul_ps(Load(value), Load(scale)), Load(addend)));
	}

	/** Computes a four-lane dot product with SSE. */
	float SimdFloat4Math::Dot(const SimdFloat4& left, const SimdFloat4& right) noexcept
	{
		const __m128 product = _mm_mul_ps(Load(left), Load(right));
		const __m128 shuffled = _mm_shuffle_ps(product, product, _MM_SHUFFLE(2, 3, 0, 1));
		const __m128 pair_sum = _mm_add_ps(product, shuffled);
		const __m128 high_sum = _mm_movehl_ps(shuffled, pair_sum);
		return _mm_cvtss_f32(_mm_add_ss(pair_sum, high_sum));
	}
}
