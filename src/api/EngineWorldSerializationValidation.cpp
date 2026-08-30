#include "EngineWorldSerializationValidation.h"

#include "EngineConfigEnumChecks.h"
#include "EngineConfigStartupEnumChecks.h"

#include <algorithm>

namespace voxel::detail
{
	bool IsWorldConfigSerializable(const WorldConfig& world) noexcept
	{
		using namespace config_validation;
		if (!IsKnownPublicTerrainGenerator(world.terrain_generator) ||
			!IsKnownPublicTerrainBiome(world.terrain_biome) ||
			!IsKnownPublicTerrainPalette(world.terrain_palette))
		{
			return false;
		}

		return std::ranges::all_of(world.edits, [](const WorldEdit& edit)
		{
			return IsKnownPublicWorldEditKind(edit.kind) &&
				IsKnownPublicBlock(edit.block);
		});
	}
}
