#pragma once

#include "BlockRegistry.h"
#include "ChunkMeshInput.h"
#include "ChunkMeshTypes.h"

#include <array>
#include <vector>

namespace ve::world::mesh
{
	struct GreedyMeshAxisPlan
	{
		ve::blocks::BlockFace block_face;
		MeshFaceDirection direction;
		int normal_axis;
		int normal_sign;
		int u_axis;
		int v_axis;
	};

	struct GreedyMeshingRules
	{
		[[nodiscard]] static constexpr std::array<GreedyMeshAxisPlan, 6> AxisPlans() noexcept
		{
			return {{
				{ ve::blocks::BlockFace::Top, MeshFaceDirection::Top, 1, 1, 0, 2 },
				{ ve::blocks::BlockFace::Bottom, MeshFaceDirection::Bottom, 1, -1, 0, 2 },
				{ ve::blocks::BlockFace::Front, MeshFaceDirection::Front, 2, 1, 0, 1 },
				{ ve::blocks::BlockFace::Back, MeshFaceDirection::Back, 2, -1, 0, 1 },
				{ ve::blocks::BlockFace::Right, MeshFaceDirection::Right, 0, 1, 2, 1 },
				{ ve::blocks::BlockFace::Left, MeshFaceDirection::Left, 0, -1, 2, 1 }
			}};
		}
	};

	class GreedyChunkMesher
	{
	public:
		/**
		 * Creates a greedy mesher over chunk block views.
		 *
		 * @param mesh_input Center chunk block view.
		 * @param block_registry Block metadata used for visibility and textures.
		 * @param neighbors Neighbor block views used for border culling.
		 */
		GreedyChunkMesher(
			const ChunkMeshInput& mesh_input,
			const ve::blocks::BlockRegistry& block_registry,
			const NeighborMeshInputs& neighbors);

		/**
		 * Appends merged visible faces for the chunk.
		 *
		 * @param faces Destination face list.
		 * @param diagnostics Optional counters filled while collecting faces.
		 */
		void CollectFaces(std::vector<MeshFace>& faces, ChunkMeshBuildDiagnostics* diagnostics = nullptr) const;

	private:
		struct MaskCell
		{
			bool has_source_block_face = false;
			bool visible = false;
			ve::blocks::BlockId block_id = ve::blocks::BlockId::Air;
			MeshFaceMaterialPayload material{};
		};

		void CollectAxis(const GreedyMeshAxisPlan& axis_plan, std::vector<MeshFace>& faces, ChunkMeshBuildDiagnostics* diagnostics) const;
		MaskCell BuildMaskCell(const GreedyMeshAxisPlan& axis_plan, int normal_coordinate, int u_coordinate, int v_coordinate) const;
		MeshFace BuildFace(
			const GreedyMeshAxisPlan& axis_plan,
			const MaskCell& cell,
			int normal_coordinate,
			int u_coordinate,
			int v_coordinate,
			int width,
			int height) const;
		ve::blocks::BlockId ReadBlock(int local_block_x, int local_block_y, int local_block_z) const;
		static bool CanMerge(const MaskCell& left, const MaskCell& right);
		static void ClearMergedCells(
			std::vector<MaskCell>& mask,
			int u_axis_block_count,
			int u_coordinate,
			int v_coordinate,
			int width,
			int height);

		const ChunkMeshInput& mesh_input_;
		const ve::blocks::BlockRegistry& block_registry_;
		const NeighborMeshInputs& neighbors_;
	};
}
