#include "WorldConfiguration.h"

namespace ve::world
{
	/// Calculates chunk storage needed by a square world.
	std::size_t ChunkCapacityForSquareWorld(int worldSizeChunks) noexcept
	{
		if (worldSizeChunks <= 0)
		{
			return 0;
		}
		return static_cast<std::size_t>(worldSizeChunks) * static_cast<std::size_t>(worldSizeChunks);
	}

	/// Builds creation settings sized for a square world.
	WorldCreateInfo CreateInfoForSquareWorld(int worldSizeChunks) noexcept
	{
		return WorldCreateInfo{ ChunkCapacityForSquareWorld(worldSizeChunks) };
	}
}
