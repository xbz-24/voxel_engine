#pragma once

namespace voxel
{
	struct BlockCoordinate
	{
		int value = 0;
		[[nodiscard]] explicit constexpr operator int() const noexcept { return value; }
	};

	struct ChunkCoordinate
	{
		int value = 0;
		[[nodiscard]] explicit constexpr operator int() const noexcept { return value; }
	};

	struct ChunkCount
	{
		int value = 0;
		[[nodiscard]] explicit constexpr operator int() const noexcept { return value; }
	};

	[[nodiscard]] constexpr BlockCoordinate BlockCoord(int value) noexcept { return { value }; }
	[[nodiscard]] constexpr ChunkCoordinate ChunkCoord(int value) noexcept { return { value }; }
	[[nodiscard]] constexpr ChunkCount Chunks(int value) noexcept { return { value }; }
	[[nodiscard]] constexpr int BlockCoordinateValue(BlockCoordinate coordinate) noexcept { return coordinate.value; }
	[[nodiscard]] constexpr int ChunkCoordinateValue(ChunkCoordinate coordinate) noexcept { return coordinate.value; }
	[[nodiscard]] constexpr int ChunkCountValue(ChunkCount count) noexcept { return count.value; }

	/** Integer block position in world coordinates. */
	struct BlockPosition
	{
		int x = 0;
		int y = 0;
		int z = 0;
	};

	[[nodiscard]] constexpr BlockPosition At(int block_x, int block_y, int block_z) noexcept
	{
		return { block_x, block_y, block_z };
	}

	[[nodiscard]] constexpr BlockPosition At(
		BlockCoordinate block_x, BlockCoordinate block_y, BlockCoordinate block_z) noexcept
	{
		return { block_x.value, block_y.value, block_z.value };
	}
}
