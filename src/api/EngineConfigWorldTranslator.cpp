#include "EngineConfigTranslatorInternal.h"
#include "EngineConfigEnumChecks.h"

namespace voxel::detail::config_translation
{
	std::optional<ve::engine::WorldBlockEdit> TryToInternalWorldEdit(const WorldEdit& edit)
	{
		if (!config_validation::IsKnownPublicBlock(edit.block)) return std::nullopt;
		const ve::blocks::BlockId block = ToInternalBlock(edit.block);
		switch (edit.kind)
		{
		case WorldEdit::Kind::SetBlock:
			return ve::world::MakeSingleBlockEdit(
				edit.position.x, edit.position.y, edit.position.z, block);
		case WorldEdit::Kind::FillBox:
			return ve::world::MakeBlockBoxEdit(
				edit.box.minimum.x,
				edit.box.minimum.y,
				edit.box.minimum.z,
				edit.box.maximum.x,
				edit.box.maximum.y,
				edit.box.maximum.z,
				block);
		default:
			return std::nullopt;
		}
	}

	ve::world::TerrainGeneratorKind ToInternalTerrainGenerator(TerrainGenerator generator) noexcept
	{
		switch (generator)
		{
		case TerrainGenerator::Flat:
			return ve::world::TerrainGeneratorKind::Flat;
		case TerrainGenerator::Procedural:
		default:
			return ve::world::TerrainGeneratorKind::Procedural;
		}
	}

	ve::world::TerrainSpawnBiome ToInternalTerrainBiome(TerrainBiome biome) noexcept
	{
		switch (biome)
		{
		case TerrainBiome::Desert:
			return ve::world::TerrainSpawnBiome::Desert;
		case TerrainBiome::Alpine:
			return ve::world::TerrainSpawnBiome::Alpine;
		case TerrainBiome::Temperate:
		default:
			return ve::world::TerrainSpawnBiome::Temperate;
		}
	}

	ve::world::TerrainBlockPalette ToInternalTerrainPalette(TerrainPalette palette) noexcept
	{
		return {
			ToInternalBlock(palette.bedrock),
			ToInternalBlock(palette.deep_stone),
			ToInternalBlock(palette.stone),
			ToInternalBlock(palette.surface),
			ToInternalBlock(palette.subsurface),
			ToInternalBlock(palette.high_elevation_surface),
			ToInternalBlock(palette.dry_surface_patch),
			ToInternalBlock(palette.gravel_patch)
		};
	}

	ve::world::TerrainGenerationSettings ToInternalTerrainGeneration(const WorldConfig& world) noexcept
	{
		return {
			world.terrain_seed,
			world.base_surface_height,
			ToInternalTerrainBiome(world.terrain_biome),
			ToInternalTerrainGenerator(world.terrain_generator),
			ToInternalTerrainPalette(world.terrain_palette)
		};
	}

	BlockPosition ToPublicBlockPosition(glm::ivec3 position) noexcept
	{
		return { position.x, position.y, position.z };
	}
}
