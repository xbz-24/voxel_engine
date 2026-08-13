#pragma once


#include "voxel/Blocks.h"

namespace voxel
{
	/** Strongly named block-space coordinate for public scene builders. */
	struct BlockCoordinate
	{
		int value = 0;

		[[nodiscard]] explicit constexpr operator int() const noexcept { return value; }
	};

	/** Strongly named chunk-space coordinate for public scene builders. */
	struct ChunkCoordinate
	{
		int value = 0;

		[[nodiscard]] explicit constexpr operator int() const noexcept { return value; }
	};

	/** Strongly named chunk count for world-size requests. */
	struct ChunkCount
	{
		int value = 0;

		[[nodiscard]] explicit constexpr operator int() const noexcept { return value; }
	};

	/** Wraps an int as a block coordinate. */
	[[nodiscard]] constexpr BlockCoordinate BlockCoord(int value) noexcept { return BlockCoordinate{ value }; }

	/** Wraps an int as a chunk coordinate. */
	[[nodiscard]] constexpr ChunkCoordinate ChunkCoord(int value) noexcept { return ChunkCoordinate{ value }; }

	/** Wraps an int as a chunk count. */
	[[nodiscard]] constexpr ChunkCount Chunks(int value) noexcept { return ChunkCount{ value }; }

	/** Returns the raw integer carried by a block coordinate. */
	[[nodiscard]] constexpr int BlockCoordinateValue(BlockCoordinate coordinate) noexcept { return coordinate.value; }

	/** Returns the raw integer carried by a chunk coordinate. */
	[[nodiscard]] constexpr int ChunkCoordinateValue(ChunkCoordinate coordinate) noexcept { return coordinate.value; }

	/** Returns the raw integer carried by a chunk count. */
	[[nodiscard]] constexpr int ChunkCountValue(ChunkCount count) noexcept { return count.value; }

	/** Integer block position in world coordinates. */
	struct BlockPosition
	{
		int x = 0;
		int y = 0;
		int z = 0;
	};

	/** Creates a block position from integer coordinates. */
	[[nodiscard]] constexpr BlockPosition At(int block_x, int block_y, int block_z) noexcept
	{
		return { block_x, block_y, block_z };
	}

	/** Creates a block position from strongly named coordinates. */
	[[nodiscard]] constexpr BlockPosition At(BlockCoordinate block_x, BlockCoordinate block_y, BlockCoordinate block_z) noexcept
	{
		return { block_x.value, block_y.value, block_z.value };
	}

	/** Inclusive block-space box used by edit commands and scene helpers. */
	struct BlockBox
	{
		BlockPosition minimum{};
		BlockPosition maximum{};

		/** Creates a normalized box from two corners. */
		[[nodiscard]] static BlockBox Between(BlockPosition first, BlockPosition second) noexcept;
	};

	/** Convenience factory for BlockBox::Between. */
	[[nodiscard]] BlockBox Box(BlockPosition first, BlockPosition second) noexcept;

	/** Public command payload for deterministic world edits. */
	struct WorldEdit
	{
		// Public command payload kept structured until opaque scene commands replace direct edit storage.
		enum class Kind
		{
			SetBlock,
			FillBox
		};

		Kind kind = Kind::SetBlock;
		BlockPosition position{};
		BlockBox box{};
		Block block = Block::Air;
	};

	/** Terrain generation strategy used when a world is initialized. */
	enum class TerrainGenerator
	{
		Procedural,
		Flat
	};

	/** Biome hint consumed by procedural terrain generation. */
	enum class TerrainBiome
	{
		Temperate,
		Desert,
		Alpine
	};

	/** Block palette used by terrain generation layers and patches. */
	struct TerrainPalette
	{
		Block bedrock = Bedrock;
		Block deep_stone = Deepslate;
		Block stone = Stone;
		Block surface = Grass;
		Block subsurface = Dirt;
		Block high_elevation_surface = Snow;
		Block dry_surface_patch = Sand;
		Block gravel_patch = Gravel;
	};
}
