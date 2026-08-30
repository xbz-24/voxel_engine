#pragma once

#include "ChunkTerrain.h"
#include "CoreTypes.h"
#include "WorldCoordinates.h"

#include <cstdint>
#include <cmath>

#include <glm/vec3.hpp>

namespace ve::world::grid
{
	[[nodiscard]] constexpr std::uint32_t SignedCoordinateBits(int coordinate) noexcept
	{
		return ve::core::ToU32(coordinate);
	}

	[[nodiscard]] constexpr std::size_t SquareChunkCapacity(int world_size_chunks) noexcept
	{
		return world_size_chunks <= 0 ? 0U : ve::core::ToIndex(world_size_chunks) * ve::core::ToIndex(world_size_chunks);
	}

	[[nodiscard]] constexpr std::size_t FlattenWorldChunkIndex(int chunk_x, int chunk_z, int world_size_chunks) noexcept
	{
		return ve::core::ToIndex((chunk_x * world_size_chunks) + chunk_z);
	}

	[[nodiscard]] constexpr bool IsInsideChunk(int local_block_x, int local_block_y, int local_block_z) noexcept
	{
		return local_block_x >= 0 && local_block_x < terrain::ChunkWidth &&
			local_block_y >= 0 && local_block_y < terrain::ChunkHeight &&
			local_block_z >= 0 && local_block_z < terrain::ChunkDepth;
	}

	[[nodiscard]] constexpr std::size_t FlattenChunkBlockIndex(
		int local_block_x,
		int local_block_y,
		int local_block_z) noexcept
	{
		return ve::core::ToIndex(
			(local_block_x * terrain::ChunkHeight + local_block_y) *
			terrain::ChunkDepth + local_block_z);
	}

	[[nodiscard]] inline int BlockCoordinateFromWorld(float coordinate) noexcept
	{
		return ve::core::ToInt(std::floor(coordinate));
	}

	[[nodiscard]] inline glm::ivec3 BlockPositionFromWorld(const glm::vec3& position) noexcept
	{
		return {
			BlockCoordinateFromWorld(position.x),
			BlockCoordinateFromWorld(position.y),
			BlockCoordinateFromWorld(position.z)
		};
	}

	[[nodiscard]] inline int ChunkCoordinateFromWorld(float coordinate, int chunk_size) noexcept
	{
		return coordinates::FloorDiv(BlockCoordinateFromWorld(coordinate), chunk_size);
	}

	[[nodiscard]] inline int ChunkXFromWorld(float world_x) noexcept
	{
		return ChunkCoordinateFromWorld(world_x, terrain::ChunkWidth);
	}

	[[nodiscard]] inline int ChunkZFromWorld(float world_z) noexcept
	{
		return ChunkCoordinateFromWorld(world_z, terrain::ChunkDepth);
	}

	[[nodiscard]] constexpr float GridBoundaryForStep(int block_coordinate, int step) noexcept
	{
		return ve::core::ToFloat(block_coordinate + (step > 0 ? 1 : 0));
	}

	[[nodiscard]] constexpr float ChunkCenterX(int chunk_x) noexcept
	{
		return ve::core::ToFloat(chunk_x * terrain::ChunkWidth) + (terrain::ChunkWidth * 0.5f);
	}

	[[nodiscard]] constexpr float ChunkCenterZ(int chunk_z) noexcept
	{
		return ve::core::ToFloat(chunk_z * terrain::ChunkDepth) + (terrain::ChunkDepth * 0.5f);
	}
}
