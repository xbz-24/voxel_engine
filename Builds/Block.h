#pragma once

#include <cstdint>

namespace ve::blocks
{
	/**
	 * Identifies the block stored in world/chunk data.
	 */
	enum class BlockId : std::uint8_t
	{
		Air = 0,
		Grass,
		Dirt,
		Stone,
		Cobblestone,
		Count
	};

	/**
	 * Names the face of a cube block.
	 */
	enum class BlockFace : std::uint8_t
	{
		Top = 0,
		Bottom,
		Front,
		Back,
		Left,
		Right,
		Count
	};
}

