#include "VulkanGpuChunkRenderer.h"

#include "BlockSolidColor.h"
#include "World.h"

#include <algorithm>
#include <array>
#include <glm/ext.hpp>

namespace ve::rendering
{
	namespace
	{
		[[nodiscard]] bool OccludesLight(const ve::world::World& world, const glm::ivec3& coordinate) noexcept
		{
			return OccludesNeighborFaces(world.GetBlock(coordinate.x, coordinate.y, coordinate.z));
		}

		[[nodiscard]] float BlockLightBoost(ve::blocks::BlockId block) noexcept
		{
			using enum ve::blocks::BlockId;
			if (block == SeaLantern) return 1.75f;
			if (block == Glass || block == Water) return 1.16f;
			if (block == GoldOre || block == DiamondOre || block == AmethystBlock) return 1.10f;
			if (block == OakLeaves || block == BirchLeaves || block == CherryLeaves || block == MossBlock) return 0.96f;
			return 1.0f;
		}

		[[nodiscard]] glm::ivec3 CornerSideOffset(const BlockFaceGeometry& face, std::size_t corner, int axis) noexcept
		{
			glm::ivec3 offset{ 0 };
			const float value = face.corners[corner][static_cast<glm::length_t>(axis)];
			offset[static_cast<glm::length_t>(axis)] = value < 0.5f ? -1 : 1;
			return offset;
		}

		[[nodiscard]] float CornerOcclusion(const ve::world::World& world,
			const BlockFaceGeometry& face,
			const glm::ivec3& block_coordinate,
			std::size_t corner) noexcept
		{
			std::array<glm::ivec3, 2> sides{};
			std::size_t side_count = 0;
			for (int axis = 0; axis < 3 && side_count < sides.size(); ++axis)
			{
				if (face.neighbor_offset[static_cast<glm::length_t>(axis)] == 0)
				{
					sides[side_count++] = CornerSideOffset(face, corner, axis);
				}
			}
			if (side_count < sides.size()) return 1.0f;
			const glm::ivec3 outside = block_coordinate + face.neighbor_offset;
			const bool side_a = OccludesLight(world, outside + sides[0]);
			const bool side_b = OccludesLight(world, outside + sides[1]);
			const bool diagonal = OccludesLight(world, outside + sides[0] + sides[1]);
			float shade = 1.0f;
			if (side_a) shade -= 0.14f;
			if (side_b) shade -= 0.14f;
			if (diagonal) shade -= (side_a && side_b) ? 0.26f : 0.18f;
			return std::clamp(shade, 0.46f, 1.0f);
		}
	}
	void VulkanGpuChunkRenderer::AppendFaceMesh(const BlockFaceGeometry& face,
		const ve::world::World& world,
		int block_x,
		int block_y,
		int block_z,
		ve::blocks::BlockId block,
		std::vector<VoxelVertex>& vertices,
		std::vector<std::uint32_t>& indices) const
	{
		const std::uint32_t first_face_vertex_index = static_cast<std::uint32_t>(vertices.size());
		const ve::blocks::SolidBlockColor color = ve::blocks::JitteredSolidColor(block, block_x, block_y, block_z);
		const glm::ivec3 block_coordinate{ block_x, block_y, block_z };
		const glm::vec3 block_origin{
			static_cast<float>(block_x),
			static_cast<float>(block_y),
			static_cast<float>(block_z)
		};
		for (std::size_t corner = 0; corner < face.corners.size(); ++corner)
		{
			const glm::vec3 vertex_position = block_origin + face.corners[corner];
			const float vertex_light = std::clamp(
				face.light * BlockLightBoost(block) * CornerOcclusion(world, face, block_coordinate, corner),
				0.20f,
				1.70f);
			vertices.push_back(VoxelVertex{
				vertex_position.x,
				vertex_position.y,
				vertex_position.z,
				color.r,
				color.g,
				color.b,
				color.a,
				vertex_light
			});
		}
		AppendQuadIndices(indices, first_face_vertex_index);
	}
}
