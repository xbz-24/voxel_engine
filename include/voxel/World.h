#pragma once

#include "voxel/Blocks.h"

#include <string>
#include <vector>

namespace voxel
{
	// TODO: Introduce typed units for block, chunk, and world coordinates so int-heavy overloads cannot be mixed accidentally.
	struct BlockPosition
	{
		int x = 0;
		int y = 0;
		int z = 0;
	};

	[[nodiscard]] constexpr BlockPosition At(int x, int y, int z) noexcept
	{
		return { x, y, z };
	}

	struct BlockBox
	{
		BlockPosition minimum{};
		BlockPosition maximum{};

		[[nodiscard]] static BlockBox Between(BlockPosition first, BlockPosition second) noexcept;
	};

	[[nodiscard]] BlockBox Box(BlockPosition first, BlockPosition second) noexcept;

	struct WorldEdit
	{
		// TODO: Hide this behind an opaque scene command list before the public API freezes.
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

	struct WorldConfig
	{
		// TODO: Add seed, terrain generator selection, biome settings, and vertical world bounds to the public world config.
		int size_chunks = 8;
		std::vector<WorldEdit> edits;

		[[nodiscard]] static WorldConfig SizeChunks(int value);

		WorldConfig& WithSizeChunks(int value) noexcept;
		WorldConfig& SetBlock(BlockPosition position, Block block);
		WorldConfig& SetBlock(int x, int y, int z, Block block);
		WorldConfig& FillBox(BlockBox box, Block block);
		WorldConfig& FillBox(BlockPosition first, BlockPosition second, Block block);
		WorldConfig& FillBox(int x1, int y1, int z1, int x2, int y2, int z2, Block block);
		WorldConfig& ClearBox(int x1, int y1, int z1, int x2, int y2, int z2);
		WorldConfig& AddFloor(int y, int radius, Block block);
		WorldConfig& AddFloor(int center_x, int y, int center_z, int radius, Block block);
		WorldConfig& AddPlatform(int y, int radius, Block block);
		WorldConfig& AddBox(int x1, int y1, int z1, int x2, int y2, int z2, Block block);
		WorldConfig& AddColumn(int x, int y, int z, int height, Block block);
		WorldConfig& AddLamp(int x, int y, int z, Block block = SeaLantern);
		WorldConfig& AddLightPost(int x, int y, int z, int height = 4, Block post = OakLog, Block light = SeaLantern);
		WorldConfig& AddPathX(int x1, int x2, int y, int z, int half_width = 1, Block block = Gravel);
		WorldConfig& AddPathZ(int x, int y, int z1, int z2, int half_width = 1, Block block = Gravel);
		WorldConfig& AddBridgeX(int x1, int x2, int y, int z, int half_width = 1, Block deck = OakPlanks, Block rail = OakLog);
		WorldConfig& AddBridgeZ(int x, int y, int z1, int z2, int half_width = 1, Block deck = OakPlanks, Block rail = OakLog);
		WorldConfig& AddHouse(int x, int y, int z, int radius = 3, Block walls = OakPlanks, Block roof = Bricks);
		WorldConfig& AddPond(int x, int y, int z, int radius = 3, Block water = Water, Block rim = MossyCobblestone);
		WorldConfig& AddGarden(int x, int y, int z, int half_width = 2, int half_depth = 3, Block soil = Dirt, Block crop = HayBlock);
		WorldConfig& AddTree(int x, int y, int z, Block trunk = OakLog, Block leaves = OakLeaves);
	};

	[[nodiscard]] WorldConfig World(int size_chunks);
	[[nodiscard]] WorldConfig World();
	[[nodiscard]] WorldConfig Scene(int size_chunks);
	[[nodiscard]] WorldConfig Scene();

	[[nodiscard]] bool SaveWorldConfig(const WorldConfig& world, const std::string& path);
	[[nodiscard]] WorldConfig LoadWorldConfig(const std::string& path);
}
