#include "GreedyChunkMesher.h"

#include <array>

namespace
{
	/// Returns the chunk dimension for a coordinate axis.
	int DimensionForAxis(int axis)
	{
		if (axis == 0) return ve::world::terrain::ChunkWidth;
		if (axis == 1) return ve::world::terrain::ChunkHeight;
		return ve::world::terrain::ChunkDepth;
	}

	/// Writes one coordinate into an xyz array.
	void SetAxisValue(std::array<int, 3>& coordinate, int axis, int value)
	{
		coordinate[static_cast<std::size_t>(axis)] = value;
	}

	/// Adds a span offset to the center coordinate for a merged face.
	void AddSpanCenter(std::array<float, 3>& center, int axis, int span)
	{
		center[static_cast<std::size_t>(axis)] += static_cast<float>(span - 1) * 0.5f;
	}
}

namespace ve::world::mesh
{
	GreedyChunkMesher::GreedyChunkMesher(const ChunkMeshInput& mesh_input, const ve::blocks::BlockRegistry& block_registry, const NeighborMeshInputs& neighbors)
		: mesh_input_(mesh_input), block_registry_(block_registry), neighbors_(neighbors)
	{
	}

	void GreedyChunkMesher::CollectFaces(std::vector<MeshFace>& faces) const
	{
		static constexpr std::array<AxisPlan, 6> axis_plans{{
			{ ve::blocks::BlockFace::Top, MeshFaceDirection::Top, 1, 1, 0, 2 },
			{ ve::blocks::BlockFace::Bottom, MeshFaceDirection::Bottom, 1, -1, 0, 2 },
			{ ve::blocks::BlockFace::Front, MeshFaceDirection::Front, 2, 1, 0, 1 },
			{ ve::blocks::BlockFace::Back, MeshFaceDirection::Back, 2, -1, 0, 1 },
			{ ve::blocks::BlockFace::Right, MeshFaceDirection::Right, 0, 1, 2, 1 },
			{ ve::blocks::BlockFace::Left, MeshFaceDirection::Left, 0, -1, 2, 1 }
		}};
		for (const AxisPlan& axis_plan : axis_plans) CollectAxis(axis_plan, faces);
	}

	GreedyChunkMesher::MaskCell GreedyChunkMesher::BuildMaskCell(const AxisPlan& axis_plan, int normal, int u, int v) const
	{
		std::array<int, 3> coordinate{};
		SetAxisValue(coordinate, axis_plan.normal_axis, normal);
		SetAxisValue(coordinate, axis_plan.u_axis, u);
		SetAxisValue(coordinate, axis_plan.v_axis, v);
		const ve::blocks::BlockId block_id = mesh_input_.GetBlock(coordinate[0], coordinate[1], coordinate[2]);
		if (block_registry_.IsAir(block_id)) return {};
		coordinate[static_cast<std::size_t>(axis_plan.normal_axis)] += axis_plan.normal_sign;
		if (block_registry_.IsSolid(ReadBlock(coordinate[0], coordinate[1], coordinate[2]))) return {};
		const bool is_grass_top = block_id == ve::blocks::BlockId::Grass && axis_plan.block_face == ve::blocks::BlockFace::Top;
		return MaskCell{ true, block_registry_.TextureFor(block_id, axis_plan.block_face), block_id, is_grass_top ? 0.404f : 1.0f, is_grass_top ? 0.655f : 1.0f, is_grass_top ? 0.239f : 1.0f };
	}

	MeshFace GreedyChunkMesher::BuildFace(const AxisPlan& axis_plan, const MaskCell& cell, int normal, int u, int v, int width, int height) const
	{
		std::array<float, 3> center{};
		center[static_cast<std::size_t>(axis_plan.normal_axis)] = static_cast<float>(normal);
		center[static_cast<std::size_t>(axis_plan.u_axis)] = static_cast<float>(u);
		center[static_cast<std::size_t>(axis_plan.v_axis)] = static_cast<float>(v);
		AddSpanCenter(center, axis_plan.u_axis, width);
		AddSpanCenter(center, axis_plan.v_axis, height);
		center[0] += static_cast<float>(mesh_input_.chunkX * terrain::ChunkWidth);
		center[2] += static_cast<float>(mesh_input_.chunkZ * terrain::ChunkDepth);
		return MeshFace{ cell.texture, axis_plan.direction, center[0], center[1], center[2], cell.red, cell.green, cell.blue, width, height };
	}

	ve::blocks::BlockId GreedyChunkMesher::ReadBlock(int x, int y, int z) const
	{
		if (y < 0 || y >= terrain::ChunkHeight) return ve::blocks::BlockId::Air;
		if (x < 0) return neighbors_.west ? neighbors_.west->GetBlock(terrain::ChunkWidth - 1, y, z) : ve::blocks::BlockId::Air;
		if (x >= terrain::ChunkWidth) return neighbors_.east ? neighbors_.east->GetBlock(0, y, z) : ve::blocks::BlockId::Air;
		if (z < 0) return neighbors_.north ? neighbors_.north->GetBlock(x, y, terrain::ChunkDepth - 1) : ve::blocks::BlockId::Air;
		if (z >= terrain::ChunkDepth) return neighbors_.south ? neighbors_.south->GetBlock(x, y, 0) : ve::blocks::BlockId::Air;
		return mesh_input_.GetBlock(x, y, z);
	}
}
