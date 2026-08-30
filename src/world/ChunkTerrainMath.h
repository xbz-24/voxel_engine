#pragma once

#include "ChunkTerrain.h"
#include "CoreTypes.h"

#include <cstdint>

namespace ve::world::terrain::detail
{
	[[nodiscard]] constexpr float WorldBlockCoordinate(
		int chunk_coordinate,
		int local_block_coordinate,
		int chunk_dimension) noexcept
	{
		return ve::core::ToFloat(local_block_coordinate + (chunk_coordinate * chunk_dimension));
	}

	[[nodiscard]] constexpr int TerrainSeedPattern(std::uint32_t terrain_seed) noexcept
	{
		return ve::core::ToInt(terrain_seed % 8191U);
	}

	[[nodiscard]] constexpr int WeightedTerrainPattern(
		float world_block_x,
		float world_block_z,
		float x_weight,
		float z_weight,
		int seed_pattern) noexcept
	{
		return ve::core::ToInt((world_block_x * x_weight) + (world_block_z * z_weight)) + seed_pattern;
	}

	[[nodiscard]] constexpr int WeightedTerrainPattern(
		float world_block_x,
		float world_block_z,
		int local_block_y,
		float x_weight,
		float z_weight,
		float y_weight,
		int seed_pattern) noexcept
	{
		return ve::core::ToInt(
			(world_block_x * x_weight) +
			(world_block_z * z_weight) +
			(ve::core::ToFloat(local_block_y) * y_weight)) + seed_pattern;
	}

	[[nodiscard]] constexpr int TerrainHeightOffset(float height_delta) noexcept
	{
		return ve::core::ToInt(height_delta);
	}

	[[nodiscard]] constexpr int BoundedSurfaceHeight(int base_height, int terrain_offset = 0) noexcept
	{
		const std::int64_t unbounded_height =
			static_cast<std::int64_t>(base_height) + static_cast<std::int64_t>(terrain_offset);
		if (unbounded_height < 1) return 1;
		if (unbounded_height > ChunkHeight - 2) return ChunkHeight - 2;
		return static_cast<int>(unbounded_height);
	}

	[[nodiscard]] constexpr int WorldBlockPatternCoordinate(float world_block_coordinate) noexcept
	{
		return ve::core::ToInt(world_block_coordinate);
	}
}
