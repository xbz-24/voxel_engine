#pragma once

#include "voxel/Blocks.h"

namespace voxel
{
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
