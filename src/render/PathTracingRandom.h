#pragma once

#include <cstdint>

namespace ve::rendering
{
	/** Fast deterministic random generator for Monte Carlo sampling. */
	class PathTracingRandom
	{
	public:
		/** @param seed Initial non-zero random state. */
		explicit PathTracingRandom(std::uint32_t seed) noexcept;

		/** @return Random integer produced by xorshift32. */
		[[nodiscard]] std::uint32_t NextU32() noexcept;

		/** @return Random float in [0, 1). */
		[[nodiscard]] float NextFloat() noexcept;

	private:
		std::uint32_t state_ = 1;
	};
}
