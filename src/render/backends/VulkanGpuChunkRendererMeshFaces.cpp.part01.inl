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
		[[nodiscard]] bool OccludesLight(
			const ve::world::World& world,
			const ve::blocks::BlockRegistry& block_registry,
			const glm::ivec3& coordinate) noexcept
		{
			const ve::blocks::BlockId block = world.GetBlock(coordinate.x, coordinate.y, coordinate.z);
			return OccludesNeighborFaces(block_registry, block);
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
			const glm::length_t glm_axis = GlmAxis(axis);
			const float value = ChunkFaceCorner(face, corner)[glm_axis];
			offset[glm_axis] = value < 0.5f ? -1 : 1;
			return offset;
		}

		[[nodiscard]] float CornerOcclusion(const ve::world::World& world,
			const ve::blocks::BlockRegistry& block_registry,
			const BlockFaceGeometry& face,
			const glm::ivec3& block_coordinate,
			std::size_t corner) noexcept
		{
			std::array<glm::ivec3, 2> sides{};
			std::size_t side_count = 0;
			for (int axis = 0; axis < 3 && side_count < sides.size(); ++axis)
			{
				if (face.neighbor_offset[GlmAxis(axis)] == 0)
				{
					sides[side_count++] = CornerSideOffset(face, corner, axis);
				}
			}
			if (side_count < sides.size()) return 1.0f;
			const glm::ivec3 outside = block_coordinate + face.neighbor_offset;
			const bool side_a = OccludesLight(world, block_registry, outside + sides[0]);
			const bool side_b = OccludesLight(world, block_registry, outside + sides[1]);
			const bool diagonal = OccludesLight(world, block_registry, outside + sides[0] + sides[1]);
			float shade = 1.0f;
			if (side_a) shade -= 0.14f;
			if (side_b) shade -= 0.14f;
			if (diagonal) shade -= (side_a && side_b) ? 0.26f : 0.18f;
			return std::clamp(shade, 0.46f, 1.0f);
		}
	}
	void VulkanGpuChunkRenderer::AppendFaceMesh(const BlockFaceGeometry& face,
		const ve::world::World& world,
		const ve::blocks::BlockRegistry& block_registry,
		int block_x,
		int block_y,
		int block_z,
		ve::blocks::BlockId block,
		std::vector<VoxelVertex>& vertices,
		std::vector<std::uint32_t>& indices) const
	{
		const std::uint32_t first_face_vertex_index = RenderElementCount(vertices.size());
		const ve::blocks::SolidBlockColor color = ve::blocks::JitteredSolidColor(block, block_x, block_y, block_z);
		const glm::ivec3 block_coordinate{ block_x, block_y, block_z };
		const glm::vec3 block_origin{
			VertexCoordinate(block_x),
			VertexCoordinate(block_y),
			VertexCoordinate(block_z)
		};
		for (std::size_t corner = 0; corner < face.offsets.size(); ++corner)
		{
