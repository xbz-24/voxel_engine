#include "GreedyChunkMesher.h"

#include <cstddef>
#include <vector>

namespace
{
	/// Returns the chunk dimension for a coordinate axis.
	int DimensionForAxis(int axis)
	{
		if (axis == 0) return ve::world::terrain::ChunkWidth;
		if (axis == 1) return ve::world::terrain::ChunkHeight;
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

	void GreedyChunkMesher::ClearMergedCells(std::vector<MaskCell>& mask, int u_dim, int u, int v, int width, int height)
	{
		for (int row = 0; row < height; row++)
		{
			for (int column = 0; column < width; column++)
			{
				mask[static_cast<std::size_t>((v + row) * u_dim + u + column)].visible = false;
			}
		}
	}

	void GreedyChunkMesher::CollectAxis(const AxisPlan& axis_plan, std::vector<MeshFace>& faces) const
	{
		const int normal_dim = DimensionForAxis(axis_plan.normal_axis);
		const int u_dim = DimensionForAxis(axis_plan.u_axis);
		const int v_dim = DimensionForAxis(axis_plan.v_axis);
		std::vector<MaskCell> mask(static_cast<std::size_t>(u_dim * v_dim));
		for (int normal = 0; normal < normal_dim; normal++)
		{
			for (int v = 0; v < v_dim; v++)
			{
				for (int u = 0; u < u_dim; u++)
				{
					mask[static_cast<std::size_t>(v * u_dim + u)] = BuildMaskCell(axis_plan, normal, u, v);
				}
			}
			for (int v = 0; v < v_dim; v++)
			{
				for (int u = 0; u < u_dim;)
				{
					const MaskCell& start = mask[static_cast<std::size_t>(v * u_dim + u)];
					if (!start.visible) { u++; continue; }
					int width = 1;
					while (u + width < u_dim && CanMerge(start, mask[static_cast<std::size_t>(v * u_dim + u + width)])) width++;
					int height = 1;
					bool can_grow = true;
					while (v + height < v_dim && can_grow)
					{
						for (int column = 0; column < width; column++)
						{
							can_grow = CanMerge(start, mask[static_cast<std::size_t>((v + height) * u_dim + u + column)]);
							if (!can_grow) break;
						}
						if (can_grow) height++;
					}
					faces.push_back(BuildFace(axis_plan, start, normal, u, v, width, height));
					ClearMergedCells(mask, u_dim, u, v, width, height);
					u += width;
				}
			}
		}
	}
}
