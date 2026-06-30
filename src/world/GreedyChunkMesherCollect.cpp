#include "GreedyChunkMesher.h"

#include <cstddef>
#include <vector>

namespace
{
	/// Returns the chunk dimension for a coordinate axis.
	int DimensionForAxis(int coordinate_axis)
	{
		if (coordinate_axis == 0) return ve::world::terrain::ChunkWidth;
		if (coordinate_axis == 1) return ve::world::terrain::ChunkHeight;
		return ve::world::terrain::ChunkDepth;
	}
}

namespace ve::world::mesh
{
	bool GreedyChunkMesher::CanMerge(const MaskCell& left, const MaskCell& right)
	{
		return left.visible && right.visible && left.texture == right.texture &&
			left.red == right.red && left.green == right.green && left.blue == right.blue;
	}

	void GreedyChunkMesher::ClearMergedCells(
		std::vector<MaskCell>& mask,
		int u_axis_block_count,
		int u_coordinate,
		int v_coordinate,
		int width,
		int height)
	{
		for (int row = 0; row < height; row++)
		{
			for (int column = 0; column < width; column++)
			{
				const std::size_t merged_cell_index =
					static_cast<std::size_t>((v_coordinate + row) * u_axis_block_count + u_coordinate + column);
				mask[merged_cell_index].visible = false;
			}
		}
	}

	void GreedyChunkMesher::CollectAxis(
		const AxisPlan& axis_plan,
		std::vector<MeshFace>& faces,
		ChunkMeshBuildDiagnostics* diagnostics) const
	{
		const int normal_axis_block_count = DimensionForAxis(axis_plan.normal_axis);
		const int u_axis_block_count = DimensionForAxis(axis_plan.u_axis);
		const int v_axis_block_count = DimensionForAxis(axis_plan.v_axis);
		std::vector<MaskCell> mask(static_cast<std::size_t>(u_axis_block_count * v_axis_block_count));
		for (int normal_coordinate = 0; normal_coordinate < normal_axis_block_count; normal_coordinate++)
		{
			for (int v_coordinate = 0; v_coordinate < v_axis_block_count; v_coordinate++)
			{
				for (int u_coordinate = 0; u_coordinate < u_axis_block_count; u_coordinate++)
				{
					MaskCell& mask_cell = mask[static_cast<std::size_t>(v_coordinate * u_axis_block_count + u_coordinate)];
					mask_cell = BuildMaskCell(axis_plan, normal_coordinate, u_coordinate, v_coordinate);
					if (diagnostics && mask_cell.has_source_block_face)
					{
						diagnostics->candidate_block_face_count++;
						if (mask_cell.visible) diagnostics->visible_block_face_count++;
						else diagnostics->culled_block_face_count++;
					}
				}
			}
			for (int v_coordinate = 0; v_coordinate < v_axis_block_count; v_coordinate++)
			{
				for (int u_coordinate = 0; u_coordinate < u_axis_block_count;)
				{
					const MaskCell& merge_start_cell =
						mask[static_cast<std::size_t>(v_coordinate * u_axis_block_count + u_coordinate)];
					if (!merge_start_cell.visible) { u_coordinate++; continue; }
					int width = 1;
					while (u_coordinate + width < u_axis_block_count &&
						CanMerge(merge_start_cell, mask[static_cast<std::size_t>(v_coordinate * u_axis_block_count + u_coordinate + width)]))
					{
						width++;
					}
					int height = 1;
					bool can_grow = true;
					while (v_coordinate + height < v_axis_block_count && can_grow)
					{
						for (int column = 0; column < width; column++)
						{
							const std::size_t next_row_cell_index =
								static_cast<std::size_t>((v_coordinate + height) * u_axis_block_count + u_coordinate + column);
							can_grow = CanMerge(merge_start_cell, mask[next_row_cell_index]);
							if (!can_grow) break;
						}
						if (can_grow) height++;
					}
					faces.push_back(BuildFace(
						axis_plan,
						merge_start_cell,
						normal_coordinate,
						u_coordinate,
						v_coordinate,
						width,
						height));
					if (diagnostics) diagnostics->merged_face_count++;
					ClearMergedCells(mask, u_axis_block_count, u_coordinate, v_coordinate, width, height);
					u_coordinate += width;
				}
			}
		}
	}
}
