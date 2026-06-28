#pragma once

#include "BlockDefinitions.h"

#include <array>
#include <cstddef>
#include <cstdint>
#include <glm/glm.hpp>
#include <vector>

namespace ve::rendering
{
	// TODO: Share face geometry, UV winding, and transparency rules with the chunk meshing pipeline instead of duplicating them for Vulkan.
	struct BlockFaceGeometry
	{
		ve::blocks::BlockFace face;
		glm::ivec3 neighbor_offset;
		std::array<glm::vec3, 4> corners;
		float light;
	};

	[[nodiscard]] inline constexpr std::array<BlockFaceGeometry, 6> ChunkFaces() noexcept
	{
		return { {
			BlockFaceGeometry{ ve::blocks::BlockFace::Top, { 0, 1, 0 }, { glm::vec3{ 0.0f, 1.0f, 0.0f }, glm::vec3{ 1.0f, 1.0f, 0.0f }, glm::vec3{ 1.0f, 1.0f, 1.0f }, glm::vec3{ 0.0f, 1.0f, 1.0f } }, 1.22f },
			BlockFaceGeometry{ ve::blocks::BlockFace::Bottom, { 0, -1, 0 }, { glm::vec3{ 0.0f, 0.0f, 1.0f }, glm::vec3{ 1.0f, 0.0f, 1.0f }, glm::vec3{ 1.0f, 0.0f, 0.0f }, glm::vec3{ 0.0f, 0.0f, 0.0f } }, 0.24f },
			BlockFaceGeometry{ ve::blocks::BlockFace::Front, { 0, 0, 1 }, { glm::vec3{ 0.0f, 0.0f, 1.0f }, glm::vec3{ 1.0f, 0.0f, 1.0f }, glm::vec3{ 1.0f, 1.0f, 1.0f }, glm::vec3{ 0.0f, 1.0f, 1.0f } }, 0.90f },
			BlockFaceGeometry{ ve::blocks::BlockFace::Back, { 0, 0, -1 }, { glm::vec3{ 1.0f, 0.0f, 0.0f }, glm::vec3{ 0.0f, 0.0f, 0.0f }, glm::vec3{ 0.0f, 1.0f, 0.0f }, glm::vec3{ 1.0f, 1.0f, 0.0f } }, 0.44f },
			BlockFaceGeometry{ ve::blocks::BlockFace::Left, { -1, 0, 0 }, { glm::vec3{ 0.0f, 0.0f, 0.0f }, glm::vec3{ 0.0f, 0.0f, 1.0f }, glm::vec3{ 0.0f, 1.0f, 1.0f }, glm::vec3{ 0.0f, 1.0f, 0.0f } }, 0.58f },
			BlockFaceGeometry{ ve::blocks::BlockFace::Right, { 1, 0, 0 }, { glm::vec3{ 1.0f, 0.0f, 1.0f }, glm::vec3{ 1.0f, 0.0f, 0.0f }, glm::vec3{ 1.0f, 1.0f, 0.0f }, glm::vec3{ 1.0f, 1.0f, 1.0f } }, 1.02f }
		} };
	}

	[[nodiscard]] inline constexpr std::array<glm::vec2, 4> ChunkFaceUvs() noexcept
	{
		return { {
			glm::vec2{ 0.0f, 1.0f },
			glm::vec2{ 1.0f, 1.0f },
			glm::vec2{ 1.0f, 0.0f },
			glm::vec2{ 0.0f, 0.0f }
		} };
	}

	[[nodiscard]] inline std::size_t FaceIndex(ve::blocks::BlockId block, ve::blocks::BlockFace face) noexcept
	{
		return (static_cast<std::size_t>(block) * static_cast<std::size_t>(ve::blocks::BlockFace::Count)) +
			static_cast<std::size_t>(face);
	}

	[[nodiscard]] inline bool IsRenderableBlock(ve::blocks::BlockId block) noexcept
	{
		return block != ve::blocks::BlockId::Air &&
			static_cast<std::size_t>(block) < static_cast<std::size_t>(ve::blocks::BlockId::Count);
	}

	[[nodiscard]] inline bool OccludesNeighborFaces(ve::blocks::BlockId block) noexcept
	{
		if (block == ve::blocks::BlockId::Glass ||
			block == ve::blocks::BlockId::Water ||
			block == ve::blocks::BlockId::OakLeaves ||
			block == ve::blocks::BlockId::BirchLeaves ||
			block == ve::blocks::BlockId::CherryLeaves)
		{
			return false;
		}
		const std::size_t index = static_cast<std::size_t>(block);
		return index < static_cast<std::size_t>(ve::blocks::BlockId::Count) &&
			ve::blocks::BuiltInBlockDefinitions[index].isSolid;
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
