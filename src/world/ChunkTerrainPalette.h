#pragma once

#include "WorldConfiguration.h"

namespace ve::world::terrain::detail
{
	[[nodiscard]] ve::world::TerrainBlockPalette ResolveTerrainPalette(
		const ve::world::TerrainGenerationSettings& terrain_generation);
}
