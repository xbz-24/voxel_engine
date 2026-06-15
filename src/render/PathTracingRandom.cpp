#include "PathTracingRandom.h"

namespace ve::rendering
{
	/** Creates a deterministic random stream. */
	PathTracingRandom::PathTracingRandom(std::uint32_t seed) noexcept
		: state_(seed == 0 ? 1 : seed)
	{
	}

	/** Advances the xorshift32 state. */
	std::uint32_t PathTracingRandom::NextU32() noexcept
	{
		state_ ^= state_ << 13;
		state_ ^= state_ >> 17;
		state_ ^= state_ << 5;
		return state_;
	}

	/** Converts the next integer into a normalized float. */
	float PathTracingRandom::NextFloat() noexcept
	{
		return static_cast<float>(NextU32() & 0x00FFFFFFu) / static_cast<float>(0x01000000u);
	}
}
