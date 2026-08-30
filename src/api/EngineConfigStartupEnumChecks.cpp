#include "EngineConfigStartupEnumChecks.h"

#include "EngineConfigEnumChecks.h"

namespace voxel::detail::config_validation
{
	bool IsKnownPublicGraphicsApi(GraphicsApi api) noexcept
	{
		switch (api)
		{
		case GraphicsApi::Vulkan:
		case GraphicsApi::OpenGLCompatibility:
		case GraphicsApi::DirectX12: return true;
		default: return false;
		}
	}

	bool IsKnownPublicLogLevel(LogLevel level) noexcept
	{
		switch (level)
		{
		case LogLevel::Default:
		case LogLevel::Trace:
		case LogLevel::Debug:
		case LogLevel::Info:
		case LogLevel::Warning:
		case LogLevel::Error:
		case LogLevel::Fatal: return true;
		default: return false;
		}
	}

	bool IsKnownPublicTerrainGenerator(TerrainGenerator generator) noexcept
	{
		switch (generator)
		{
		case TerrainGenerator::Procedural:
		case TerrainGenerator::Flat: return true;
		default: return false;
		}
	}

	bool IsKnownPublicTerrainBiome(TerrainBiome biome) noexcept
	{
		switch (biome)
		{
		case TerrainBiome::Temperate:
		case TerrainBiome::Desert:
		case TerrainBiome::Alpine: return true;
		default: return false;
		}
	}

	bool IsKnownPublicTerrainPalette(const TerrainPalette& palette) noexcept
	{
		return IsKnownPublicBlock(palette.bedrock) &&
			IsKnownPublicBlock(palette.deep_stone) && IsKnownPublicBlock(palette.stone) &&
			IsKnownPublicBlock(palette.surface) && IsKnownPublicBlock(palette.subsurface) &&
			IsKnownPublicBlock(palette.high_elevation_surface) &&
			IsKnownPublicBlock(palette.dry_surface_patch) &&
			IsKnownPublicBlock(palette.gravel_patch);
	}
}
