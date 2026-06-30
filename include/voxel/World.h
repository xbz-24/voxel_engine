#pragma once

#include "voxel/Blocks.h"

#include <cstdint>
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

	[[nodiscard]] constexpr BlockPosition At(int block_x, int block_y, int block_z) noexcept
	{
		return { block_x, block_y, block_z };
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

	enum class TerrainGenerator
	{
		Procedural,
		Flat
	};

	enum class TerrainBiome
	{
		Temperate,
		Desert,
		Alpine
	};

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

	struct WorldConfig
	{
		// TODO: Add vertical world bounds once runtime chunks are no longer fixed to ChunkHeight.
		int size_chunks = 8;
		std::uint32_t terrain_seed = 0;
		int base_surface_height = 40;
		TerrainGenerator terrain_generator = TerrainGenerator::Procedural;
		TerrainBiome terrain_biome = TerrainBiome::Temperate;
		TerrainPalette terrain_palette{};
		std::vector<WorldEdit> edits;

		[[nodiscard]] static WorldConfig SizeChunks(int value);

		WorldConfig& WithSizeChunks(int value) noexcept;
		WorldConfig& WithTerrainSeed(std::uint32_t value) noexcept;
		WorldConfig& WithBaseSurfaceHeight(int value) noexcept;
		WorldConfig& WithTerrainGenerator(TerrainGenerator value) noexcept;
		WorldConfig& WithTerrainBiome(TerrainBiome value) noexcept;
		WorldConfig& WithTerrainPalette(TerrainPalette value) noexcept;
		WorldConfig& SetBlock(BlockPosition position, Block block);
		WorldConfig& SetBlock(int block_x, int block_y, int block_z, Block block);
		WorldConfig& FillBox(BlockBox box, Block block);
		WorldConfig& FillBox(BlockPosition first, BlockPosition second, Block block);
		WorldConfig& FillBox(int first_x, int first_y, int first_z, int second_x, int second_y, int second_z, Block block);
		WorldConfig& ClearBox(int first_x, int first_y, int first_z, int second_x, int second_y, int second_z);
		WorldConfig& AddFloor(int block_y, int radius, Block block);
		WorldConfig& AddFloor(int center_x, int block_y, int center_z, int radius, Block block);
		WorldConfig& AddPlatform(int block_y, int radius, Block block);
		WorldConfig& AddBox(int first_x, int first_y, int first_z, int second_x, int second_y, int second_z, Block block);
		WorldConfig& AddColumn(int block_x, int base_y, int block_z, int height, Block block);
		WorldConfig& AddLamp(int block_x, int block_y, int block_z, Block block = SeaLantern);
		WorldConfig& AddLightPost(int block_x, int base_y, int block_z, int height = 4, Block post = OakLog, Block light = SeaLantern);
		WorldConfig& AddPathX(int start_x, int end_x, int block_y, int center_z, int half_width = 1, Block block = Gravel);
		WorldConfig& AddPathZ(int center_x, int block_y, int start_z, int end_z, int half_width = 1, Block block = Gravel);
		WorldConfig& AddBridgeX(int start_x, int end_x, int block_y, int center_z, int half_width = 1, Block deck = OakPlanks, Block rail = OakLog);
		WorldConfig& AddBridgeZ(int center_x, int block_y, int start_z, int end_z, int half_width = 1, Block deck = OakPlanks, Block rail = OakLog);
		WorldConfig& AddHouse(int center_x, int base_y, int center_z, int radius = 3, Block walls = OakPlanks, Block roof = Bricks);
		WorldConfig& AddPond(int center_x, int block_y, int center_z, int radius = 3, Block water = Water, Block rim = MossyCobblestone);
		WorldConfig& AddGarden(int center_x, int block_y, int center_z, int half_width = 2, int half_depth = 3, Block soil = Dirt, Block crop = HayBlock);
		WorldConfig& AddTree(int trunk_x, int base_y, int trunk_z, Block trunk = OakLog, Block leaves = OakLeaves);
	};

	[[nodiscard]] WorldConfig World(int size_chunks);
	[[nodiscard]] WorldConfig World();
	[[nodiscard]] WorldConfig Scene(int size_chunks);
	[[nodiscard]] WorldConfig Scene();

	[[nodiscard]] bool SaveWorldConfig(const WorldConfig& world, const std::string& path);
	[[nodiscard]] WorldConfig LoadWorldConfig(const std::string& path);
}
