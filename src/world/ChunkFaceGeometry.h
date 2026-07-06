#pragma once

#include "Block.h"
#include "ChunkFaceOffsets.h"

#include <glm/glm.hpp>

#include <span>

namespace ve::world::mesh
{
	struct ChunkFaceGeometry
	{
		ve::blocks::BlockFace block_face = ve::blocks::BlockFace::Top;
		MeshFaceDirection direction = MeshFaceDirection::Top;
		glm::ivec3 neighbor_offset{ 0 };
		FaceOffsets offsets{};
		float light = 1.0f;
	};

	/** @return Shared cube face descriptors used by CPU meshing and Vulkan chunk rendering. */
	[[nodiscard]] std::span<const ChunkFaceGeometry> ChunkFaceGeometries() noexcept;

	/**
	 * Selects the shared descriptor for one voxel face direction.
	 *
	 * @param direction Cube side requested by the caller.
	 * @return Face geometry descriptor for that direction.
	 */
	[[nodiscard]] const ChunkFaceGeometry& GeometryForDirection(MeshFaceDirection direction) noexcept;
}
