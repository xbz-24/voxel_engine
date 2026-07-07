#include "GreedyChunkMesher.h"

#include <array>

namespace
{
	/// Writes one coordinate into an xyz array.
	void SetAxisValue(std::array<int, 3>& block_coordinate, int axis, int value)
	{
		block_coordinate[static_cast<std::size_t>(axis)] = value;
	}

	/// Adds a span offset to the center coordinate for a merged face.
	void AddSpanCenter(std::array<float, 3>& face_center, int axis, int merged_face_span)
	{
		face_center[static_cast<std::size_t>(axis)] += static_cast<float>(merged_face_span - 1) * 0.5f;
	}
}

namespace ve::world::mesh
{
	GreedyChunkMesher::GreedyChunkMesher(
		const ChunkMeshInput& mesh_input,
		const ve::blocks::BlockRegistry& block_registry,
		const NeighborMeshInputs& neighbors)
		: mesh_input_(mesh_input), block_registry_(block_registry), neighbors_(neighbors)
	{
	}

	void GreedyChunkMesher::CollectFaces(std::vector<MeshFace>& faces, ChunkMeshBuildDiagnostics* diagnostics) const
	{
		static constexpr std::array<GreedyMeshAxisPlan, 6> axis_plans = GreedyMeshingRules::AxisPlans();
		for (const GreedyMeshAxisPlan& axis_plan : axis_plans) CollectAxis(axis_plan, faces, diagnostics);
	}

	GreedyChunkMesher::MaskCell GreedyChunkMesher::BuildMaskCell(
		const GreedyMeshAxisPlan& axis_plan,
		int normal_coordinate,
		int u_coordinate,
		int v_coordinate) const
	{
		std::array<int, 3> block_coordinate{};
		SetAxisValue(block_coordinate, axis_plan.normal_axis, normal_coordinate);
		SetAxisValue(block_coordinate, axis_plan.u_axis, u_coordinate);
		SetAxisValue(block_coordinate, axis_plan.v_axis, v_coordinate);
		const ve::blocks::BlockId block_id =
			mesh_input_.GetBlock(block_coordinate[0], block_coordinate[1], block_coordinate[2]);
		if (block_registry_.IsAir(block_id)) return {};
		MaskCell mask_cell{};
		mask_cell.has_source_block_face = true;
		mask_cell.block_id = block_id;
		block_coordinate[static_cast<std::size_t>(axis_plan.normal_axis)] += axis_plan.normal_sign;
		if (block_registry_.OccludesNeighborFaces(ReadBlock(block_coordinate[0], block_coordinate[1], block_coordinate[2]))) return mask_cell;
		const bool is_grass_top = block_id == ve::blocks::BlockId::Grass && axis_plan.block_face == ve::blocks::BlockFace::Top;
		mask_cell.visible = true;
		mask_cell.material.texture = block_registry_.TextureFor(block_id, axis_plan.block_face);
		mask_cell.material.red = is_grass_top ? 0.404f : 1.0f;
		mask_cell.material.green = is_grass_top ? 0.655f : 1.0f;
		mask_cell.material.blue = is_grass_top ? 0.239f : 1.0f;
		return mask_cell;
	}

	MeshFace GreedyChunkMesher::BuildFace(
		const GreedyMeshAxisPlan& axis_plan,
		const MaskCell& cell,
		int normal_coordinate,
		int u_coordinate,
		int v_coordinate,
		int width,
		int height) const
	{
		std::array<float, 3> face_center{};
		face_center[static_cast<std::size_t>(axis_plan.normal_axis)] = static_cast<float>(normal_coordinate);
		face_center[static_cast<std::size_t>(axis_plan.u_axis)] = static_cast<float>(u_coordinate);
		face_center[static_cast<std::size_t>(axis_plan.v_axis)] = static_cast<float>(v_coordinate);
		AddSpanCenter(face_center, axis_plan.u_axis, width);
		AddSpanCenter(face_center, axis_plan.v_axis, height);
		face_center[0] += static_cast<float>(mesh_input_.chunkX * terrain::ChunkWidth);
		face_center[2] += static_cast<float>(mesh_input_.chunkZ * terrain::ChunkDepth);
		return MeshFace{
			cell.material,
			axis_plan.direction,
			face_center[0],
			face_center[1],
			face_center[2],
			width,
			height
		};
	}

}
