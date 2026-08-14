#include "voxel/Engine.h"

#include "EngineWorldGeometryMath.h"

namespace voxel
{
	WorldConfig& WorldConfig::AddLightPost(
		int block_x, int base_y, int block_z, int height, Block post, Block light)
	{
		if (height <= 0)
		{
			return AddLamp(block_x, base_y, block_z, light);
		}
		int post_top_y = 0;
		int light_y = 0;
		if (!detail::TryBlockCoordinate(
				detail::WidenBlockCoordinate(base_y) + detail::WidenBlockCoordinate(height) - 1,
				post_top_y)
			|| !detail::TryBlockCoordinate(
				detail::WidenBlockCoordinate(base_y) + detail::WidenBlockCoordinate(height),
				light_y))
		{
			return *this;
		}
		return FillBox(block_x, base_y, block_z, block_x, post_top_y, block_z, post)
			.SetBlock(block_x, light_y, block_z, light);
	}

	WorldConfig& WorldConfig::AddHouse(
		int center_x, int base_y, int center_z, int radius, Block walls, Block roof)
	{
		int left = 0, right = 0, front = 0, back = 0;
		int wall_base = 0, door_top = 0, wall_top = 0, roof_y = 0;
		int roof_left = 0, roof_right = 0, roof_front = 0, roof_back = 0;
		int door_left = 0, door_right = 0, inner_front = 0, inner_back = 0;
		if (!detail::TryCenteredBlockSpan(center_x, radius, left, right)
			|| !detail::TryCenteredBlockSpan(center_z, radius, front, back)
			|| !detail::TryBlockCoordinate(detail::WidenBlockCoordinate(base_y) + 1, wall_base)
			|| !detail::TryBlockCoordinate(detail::WidenBlockCoordinate(base_y) + 2, door_top)
			|| !detail::TryBlockCoordinate(detail::WidenBlockCoordinate(base_y) + 3, wall_top)
			|| !detail::TryBlockCoordinate(detail::WidenBlockCoordinate(base_y) + 4, roof_y)
			|| !detail::TryBlockCoordinate(detail::WidenBlockCoordinate(left) - 1, roof_left)
			|| !detail::TryBlockCoordinate(detail::WidenBlockCoordinate(right) + 1, roof_right)
			|| !detail::TryBlockCoordinate(detail::WidenBlockCoordinate(front) - 1, roof_front)
			|| !detail::TryBlockCoordinate(detail::WidenBlockCoordinate(back) + 1, roof_back)
			|| !detail::TryCenteredBlockSpan(center_x, 1, door_left, door_right)
			|| !detail::TryCenteredBlockSpan(center_z, 1, inner_front, inner_back))
		{
			return *this;
		}
		return AddBox(left, base_y, front, right, base_y, back, walls)
			.AddBox(left, wall_base, front, right, wall_top, front, walls)
			.AddBox(left, wall_base, back, right, wall_top, back, walls)
			.AddBox(left, wall_base, front, left, wall_top, back, walls)
			.AddBox(right, wall_base, front, right, wall_top, back, walls)
			.AddBox(roof_left, roof_y, roof_front, roof_right, roof_y, roof_back, roof)
			.ClearBox(door_left, wall_base, front, door_right, door_top, front)
			.ClearBox(door_left, wall_base, inner_front, door_right, wall_top, inner_back)
			.AddLamp(center_x, wall_top, center_z);
	}
}
