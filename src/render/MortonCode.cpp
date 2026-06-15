#include "MortonCode.h"

namespace ve::rendering
{
	namespace
	{
		/** @param value 21-bit integer. @return Bits spread apart by two zero bits. */
		std::uint64_t SpreadByTwoBits(std::uint32_t value) noexcept
		{
			std::uint64_t spread = value & 0x1fffffULL;
			spread = (spread | (spread << 32U)) & 0x1f00000000ffffULL;
			spread = (spread | (spread << 16U)) & 0x1f0000ff0000ffULL;
			spread = (spread | (spread << 8U)) & 0x100f00f00f00f00fULL;
			spread = (spread | (spread << 4U)) & 0x10c30c30c30c30c3ULL;
			spread = (spread | (spread << 2U)) & 0x1249249249249249ULL;
			return spread;
		}

		/** @param value Interleaved Morton bits. @return Compact 21-bit integer. */
		std::uint32_t CompactByTwoBits(std::uint64_t value) noexcept
		{
			value &= 0x1249249249249249ULL;
			value = (value ^ (value >> 2U)) & 0x10c30c30c30c30c3ULL;
			value = (value ^ (value >> 4U)) & 0x100f00f00f00f00fULL;
			value = (value ^ (value >> 8U)) & 0x1f0000ff0000ffULL;
			value = (value ^ (value >> 16U)) & 0x1f00000000ffffULL;
			value = (value ^ (value >> 32U)) & 0x1fffffULL;
			return static_cast<std::uint32_t>(value);
		}
	}

	/** Encodes a 3D coordinate into a spatially sortable Morton code. */
	std::uint64_t EncodeMorton3D(MortonCoordinate coordinate) noexcept
	{
		return SpreadByTwoBits(coordinate.x) | (SpreadByTwoBits(coordinate.y) << 1U) | (SpreadByTwoBits(coordinate.z) << 2U);
	}

	/** Decodes a 64-bit Morton code back into a 3D coordinate. */
	MortonCoordinate DecodeMorton3D(std::uint64_t code) noexcept
	{
		return { CompactByTwoBits(code), CompactByTwoBits(code >> 1U), CompactByTwoBits(code >> 2U) };
	}
}
