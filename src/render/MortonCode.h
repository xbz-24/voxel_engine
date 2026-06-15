#pragma once

#include <cstdint>

namespace ve::rendering
{
	struct MortonCoordinate
	{
		std::uint32_t x = 0;
		std::uint32_t y = 0;
		std::uint32_t z = 0;
	};

	/** @param coordinate 3D grid coordinate with components up to 21 bits. @return 64-bit Morton/Z-order code. */
	[[nodiscard]] std::uint64_t EncodeMorton3D(MortonCoordinate coordinate) noexcept;

	/** @param code 64-bit Morton/Z-order code. @return Decoded 3D grid coordinate. */
	[[nodiscard]] MortonCoordinate DecodeMorton3D(std::uint64_t code) noexcept;
}
