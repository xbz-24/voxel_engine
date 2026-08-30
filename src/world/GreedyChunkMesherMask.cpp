#include "GreedyChunkMesher.h"

#include "CoreTypes.h"

#include <cstddef>

namespace ve::world::mesh
{
	bool GreedyChunkMesher::CanMerge(const MaskCell& left, const MaskCell& right)
	{
		return left.visible && right.visible && left.material == right.material;
	}

	void GreedyChunkMesher::ClearMergedCells(
		std::vector<MaskCell>& mask,
		int u_axis_block_count,
		int u_coordinate,
		int v_coordinate,
		int width,
		int height)
	{
		for (int row = 0; row < height; ++row)
		{
			for (int column = 0; column < width; ++column)
			{
				const std::size_t index = ve::core::ToIndex(
					(v_coordinate + row) * u_axis_block_count + u_coordinate + column);
				mask[index].visible = false;
			}
		}
	}
}
