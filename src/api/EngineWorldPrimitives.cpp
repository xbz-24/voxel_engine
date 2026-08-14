#include "voxel/Engine.h"

#include "EngineWorldGeometryMath.h"

namespace voxel
{
	WorldConfig& WorldConfig::AddFloor(int block_y, int radius, Block block)
	{
		return AddFloor(0, block_y, 0, radius, block);
	}

	WorldConfig& WorldConfig::AddFloor(
		int center_x,
		int block_y,
		int center_z,
		int radius,
		Block block)
	{
		int first_x = 0;
		int second_x = 0;
		int first_z = 0;
		int second_z = 0;
		if (!detail::TryCenteredBlockSpan(center_x, radius, first_x, second_x)
			|| !detail::TryCenteredBlockSpan(center_z, radius, first_z, second_z))
		{
			return *this;
		}
		return FillBox(first_x, block_y, first_z, second_x, block_y, second_z, block);
	}

	WorldConfig& WorldConfig::AddPlatform(int block_y, int radius, Block block)
	{
		return AddFloor(block_y, radius, block);
	}

	WorldConfig& WorldConfig::AddBox(
		int first_x, int first_y, int first_z,
		int second_x, int second_y, int second_z,
		Block block)
	{
		return FillBox(first_x, first_y, first_z, second_x, second_y, second_z, block);
	}

	WorldConfig& WorldConfig::AddColumn(
		int block_x, int base_y, int block_z, int height, Block block)
	{
		int top_y = 0;
		if (height <= 0
			|| !detail::TryBlockCoordinate(
				detail::WidenBlockCoordinate(base_y) + detail::WidenBlockCoordinate(height) - 1,
				top_y))
		{
			return *this;
		}
		return FillBox(block_x, base_y, block_z, block_x, top_y, block_z, block);
	}

	WorldConfig& WorldConfig::AddLamp(int block_x, int block_y, int block_z, Block block)
	{
		return SetBlock(block_x, block_y, block_z, block);
	}
}
