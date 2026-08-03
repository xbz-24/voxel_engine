
#include "voxel/WorldTypes.h"

#include <cstdint>
#include <string>
#include <vector>

namespace voxel
{
	/** Public world-generation and authored-edit configuration. */
	struct WorldConfig
	{
		// Height currently follows the fixed runtime chunk height; vertical bounds are tracked in the architecture roadmap.
		int size_chunks = 8;
		std::uint32_t terrain_seed = 0;
		int base_surface_height = 40;
		TerrainGenerator terrain_generator = TerrainGenerator::Procedural;
		TerrainBiome terrain_biome = TerrainBiome::Temperate;
		TerrainPalette terrain_palette{};
		std::vector<WorldEdit> edits;

		/** Creates a world config with a square chunk count. */
		[[nodiscard]] static WorldConfig SizeChunks(int value);

		/** Creates a world config with a strongly named square chunk count. */
		[[nodiscard]] static WorldConfig SizeChunks(ChunkCount value);

		/** Sets square world size in chunks. */
		WorldConfig& WithSizeChunks(int value) noexcept;

		/** Sets square world size in chunks. */
		WorldConfig& WithSizeChunks(ChunkCount value) noexcept;

		/** Sets the procedural terrain seed. */
		WorldConfig& WithTerrainSeed(std::uint32_t value) noexcept;

		/** Sets the base terrain surface height in blocks. */
		WorldConfig& WithBaseSurfaceHeight(int value) noexcept;

		/** Sets the terrain generation strategy. */
		WorldConfig& WithTerrainGenerator(TerrainGenerator value) noexcept;

		/** Sets the procedural biome hint. */
		WorldConfig& WithTerrainBiome(TerrainBiome value) noexcept;

		/** Sets the terrain block palette. */
		WorldConfig& WithTerrainPalette(TerrainPalette value) noexcept;

		/** Adds a single-block edit command. */
		WorldConfig& SetBlock(BlockPosition position, Block block);

		/** Adds a single-block edit command by coordinates. */
		WorldConfig& SetBlock(int block_x, int block_y, int block_z, Block block);

		/** Adds a filled-box edit command. */
		WorldConfig& FillBox(BlockBox box, Block block);

		/** Adds a filled-box edit command between inclusive corners. */
		WorldConfig& FillBox(BlockPosition first, BlockPosition second, Block block);

		/** Adds a filled-box edit command by inclusive corner coordinates. */
		WorldConfig& FillBox(int first_x, int first_y, int first_z, int second_x, int second_y, int second_z, Block block);

		/** Adds an air fill edit command by inclusive corner coordinates. */
		WorldConfig& ClearBox(int first_x, int first_y, int first_z, int second_x, int second_y, int second_z);

		/** Adds a centered floor at y with radius. */
		WorldConfig& AddFloor(int block_y, int radius, Block block);

		/** Adds a centered floor at an explicit x/z center. */
		WorldConfig& AddFloor(int center_x, int block_y, int center_z, int radius, Block block);

		/** Adds a simple platform helper. */
		WorldConfig& AddPlatform(int block_y, int radius, Block block);

		/** Adds a solid box helper. */
		WorldConfig& AddBox(int first_x, int first_y, int first_z, int second_x, int second_y, int second_z, Block block);

		/** Adds a vertical column helper. */
		WorldConfig& AddColumn(int block_x, int base_y, int block_z, int height, Block block);

		/** Adds a single lamp block helper. */
		WorldConfig& AddLamp(int block_x, int block_y, int block_z, Block block = SeaLantern);

		/** Adds a vertical post with a light block at the top. */
		WorldConfig& AddLightPost(int block_x, int base_y, int block_z, int height = 4, Block post = OakLog, Block light = SeaLantern);

		/** Adds an x-axis path strip. */
