#pragma once

#include "BlockRegistry.h"
#include "ChunkMeshInput.h"
#include "ChunkMeshTypes.h"

#include <vector>

namespace ve::world::mesh
{
	// TODO: Share greedy meshing rules with any future compute mesher so CPU/GPU chunk output remains visually identical.
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
		GreedyChunkMesher(const ChunkMeshInput& mesh_input, const ve::blocks::BlockRegistry& block_registry, const NeighborMeshInputs& neighbors);

		/**
		 * Appends merged visible faces for the chunk.
		 *
		 * @param faces Destination face list.
		 */
		void CollectFaces(std::vector<MeshFace>& faces) const;

	private:
		struct AxisPlan
		{
			ve::blocks::BlockFace block_face;
			MeshFaceDirection direction;
			int normal_axis;
			int normal_sign;
			int u_axis;
			int v_axis;
		};

		struct MaskCell
		{
			// TODO: Replace per-cell RGB with material/light payload once lighting data is generated per block face.
			bool visible = false;
			ve::rendering::TextureHandle texture = ve::rendering::kInvalidTextureHandle;
			ve::blocks::BlockId block_id = ve::blocks::BlockId::Air;
			float red = 1.0f;
			float green = 1.0f;
			float blue = 1.0f;
		};

		void CollectAxis(const AxisPlan& axis_plan, std::vector<MeshFace>& faces) const;
		MaskCell BuildMaskCell(const AxisPlan& axis_plan, int normal, int u, int v) const;
		MeshFace BuildFace(const AxisPlan& axis_plan, const MaskCell& cell, int normal, int u, int v, int width, int height) const;
		ve::blocks::BlockId ReadBlock(int x, int y, int z) const;
		static bool CanMerge(const MaskCell& left, const MaskCell& right);
		static void ClearMergedCells(std::vector<MaskCell>& mask, int u_dim, int u, int v, int width, int height);

		const ChunkMeshInput& mesh_input_;
		const ve::blocks::BlockRegistry& block_registry_;
		const NeighborMeshInputs& neighbors_;
	};
}
