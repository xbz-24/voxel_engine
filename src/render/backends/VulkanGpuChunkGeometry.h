#pragma once

#include "BlockRegistry.h"
#include "ChunkFaceGeometry.h"

#include <cstddef>
#include <cstdint>
#include <glm/glm.hpp>
#include <span>
#include <vector>

namespace ve::rendering
{
	using BlockFaceGeometry = ve::world::mesh::ChunkFaceGeometry;

	[[nodiscard]] inline std::span<const BlockFaceGeometry> ChunkFaces() noexcept
	{
		return ve::world::mesh::ChunkFaceGeometries();
	}

	[[nodiscard]] inline glm::vec3 ChunkFaceCorner(const BlockFaceGeometry& face, std::size_t corner) noexcept
	{
		const ve::world::mesh::FaceVertexOffset& offset = face.offsets[corner];
		return glm::vec3{
			offset.local_position_x + 0.5f,
			offset.local_position_y + 0.5f,
			offset.local_position_z + 0.5f
		};
	}

	[[nodiscard]] inline std::size_t FaceIndex(ve::blocks::BlockId block, ve::blocks::BlockFace face) noexcept
	{
		return (static_cast<std::size_t>(block) * static_cast<std::size_t>(ve::blocks::BlockFace::Count)) +
			static_cast<std::size_t>(face);
	}

	[[nodiscard]] inline bool IsRenderableBlock(const ve::blocks::BlockRegistry& block_registry, ve::blocks::BlockId block) noexcept
	{
		return block_registry.IsRenderable(block);
	}

	[[nodiscard]] inline bool OccludesNeighborFaces(const ve::blocks::BlockRegistry& block_registry, ve::blocks::BlockId block) noexcept
	{
		return block_registry.OccludesNeighborFaces(block);
	}

	inline void AppendQuadIndices(std::vector<std::uint32_t>& indices, std::uint32_t base)
	{
		indices.push_back(base);
		indices.push_back(base + 1u);
		indices.push_back(base + 2u);
		indices.push_back(base);
		indices.push_back(base + 2u);
		indices.push_back(base + 3u);
	}
}
