#include "EngineConfigStartupEnumChecks.h"
#include "EngineConfigValidationInternal.h"

namespace voxel::detail::config_validation
{
	void ValidateTerrainConfiguration(const EngineConfig& config,
		std::vector<std::string>& issues)
	{
		if (!IsKnownPublicTerrainGenerator(config.world.terrain_generator))
		{
			issues.push_back("world.terrain_generator is not a known terrain generator");
		}
		if (!IsKnownPublicTerrainBiome(config.world.terrain_biome))
		{
			issues.push_back("world.terrain_biome is not a known terrain biome");
		}
		if (!IsKnownPublicTerrainPalette(config.world.terrain_palette))
		{
			issues.push_back("world.terrain_palette contains an unknown block");
		}
	}
}
