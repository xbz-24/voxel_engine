#include "voxel/Engine.h"

#include "EngineWorldGeometryMath.h"

namespace voxel
{
	WorldConfig& WorldConfig::AddPond(
		int center_x, int block_y, int center_z, int radius, Block water, Block rim)
	{
		int left = 0, right = 0, front = 0, back = 0;
		int inner_left = 0, inner_right = 0, inner_front = 0, inner_back = 0;
		int clear_bottom_y = 0, clear_top_y = 0;
		if (!detail::TryCenteredBlockSpan(center_x, radius, left, right)
			|| !detail::TryCenteredBlockSpan(center_z, radius, front, back)
			|| !detail::TryBlockCoordinate(detail::WidenBlockCoordinate(left) + 1, inner_left)
			|| !detail::TryBlockCoordinate(detail::WidenBlockCoordinate(right) - 1, inner_right)
			|| !detail::TryBlockCoordinate(detail::WidenBlockCoordinate(front) + 1, inner_front)
			|| !detail::TryBlockCoordinate(detail::WidenBlockCoordinate(back) - 1, inner_back)
			|| !detail::TryBlockCoordinate(detail::WidenBlockCoordinate(block_y) + 1, clear_bottom_y)
			|| !detail::TryBlockCoordinate(detail::WidenBlockCoordinate(block_y) + 3, clear_top_y))
		{
			return *this;
		}
		return FillBox(left, block_y, front, right, block_y, back, rim)
			.FillBox(inner_left, block_y, inner_front, inner_right, block_y, inner_back, water)
			.ClearBox(inner_left, clear_bottom_y, inner_front, inner_right, clear_top_y, inner_back);
	}

	WorldConfig& WorldConfig::AddGarden(
		int center_x, int block_y, int center_z, int half_width, int half_depth,
		Block soil, Block crop)
	{
		int left = 0, right = 0, front = 0, back = 0;
		int fence_left = 0, fence_right = 0, first_fence_z = 0, second_fence_z = 0;
		int crop_y = 0;
		if (!detail::TryCenteredBlockSpan(center_x, half_width, left, right)
			|| !detail::TryCenteredBlockSpan(center_z, half_depth, front, back)
			|| !detail::TryBlockCoordinate(detail::WidenBlockCoordinate(left) - 1, fence_left)
			|| !detail::TryBlockCoordinate(detail::WidenBlockCoordinate(right) + 1, fence_right)
			|| !detail::TryBlockCoordinate(detail::WidenBlockCoordinate(front) - 1, first_fence_z)
			|| !detail::TryBlockCoordinate(detail::WidenBlockCoordinate(back) + 1, second_fence_z)
			|| !detail::TryBlockCoordinate(detail::WidenBlockCoordinate(block_y) + 1, crop_y))
		{
			return *this;
		}
		return FillBox(left, block_y, front, right, block_y, back, soil)
			.FillBox(left, crop_y, front, right, crop_y, back, crop)
			.FillBox(fence_left, block_y, first_fence_z, fence_right, block_y, first_fence_z, OakLog)
			.FillBox(fence_left, block_y, second_fence_z, fence_right, block_y, second_fence_z, OakLog);
	}

	WorldConfig& WorldConfig::AddTree(
		int trunk_x, int base_y, int trunk_z, Block trunk, Block leaves)
	{
		int canopy_left = 0, canopy_right = 0, canopy_front = 0, canopy_back = 0;
		int crown_left = 0, crown_right = 0, crown_front = 0, crown_back = 0;
		int lower_canopy_y = 0, upper_canopy_y = 0, crown_y = 0;
		if (!detail::TryCenteredBlockSpan(trunk_x, 2, canopy_left, canopy_right)
			|| !detail::TryCenteredBlockSpan(trunk_z, 2, canopy_front, canopy_back)
			|| !detail::TryCenteredBlockSpan(trunk_x, 1, crown_left, crown_right)
			|| !detail::TryCenteredBlockSpan(trunk_z, 1, crown_front, crown_back)
			|| !detail::TryBlockCoordinate(detail::WidenBlockCoordinate(base_y) + 3, lower_canopy_y)
			|| !detail::TryBlockCoordinate(detail::WidenBlockCoordinate(base_y) + 5, upper_canopy_y)
			|| !detail::TryBlockCoordinate(detail::WidenBlockCoordinate(base_y) + 6, crown_y))
		{
			return *this;
		}
		return AddColumn(trunk_x, base_y, trunk_z, 5, trunk)
			.AddBox(canopy_left, lower_canopy_y, canopy_front,
				canopy_right, upper_canopy_y, canopy_back, leaves)
			.AddBox(crown_left, crown_y, crown_front, crown_right, crown_y, crown_back, leaves);
	}
}
