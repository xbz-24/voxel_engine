#include "voxel/Engine.h"

#include "EngineWorldGeometryMath.h"

namespace voxel
{
	WorldConfig& WorldConfig::AddPathX(
		int start_x, int end_x, int block_y, int center_z, int half_width, Block block)
	{
		int first_z = 0;
		int second_z = 0;
		if (!detail::TryCenteredBlockSpan(center_z, half_width, first_z, second_z))
		{
			return *this;
		}
		return FillBox(start_x, block_y, first_z, end_x, block_y, second_z, block);
	}

	WorldConfig& WorldConfig::AddPathZ(
		int center_x, int block_y, int start_z, int end_z, int half_width, Block block)
	{
		int first_x = 0;
		int second_x = 0;
		if (!detail::TryCenteredBlockSpan(center_x, half_width, first_x, second_x))
		{
			return *this;
		}
		return FillBox(first_x, block_y, start_z, second_x, block_y, end_z, block);
	}

	WorldConfig& WorldConfig::AddBridgeX(
		int start_x, int end_x, int block_y, int center_z,
		int half_width, Block deck, Block rail)
	{
		int deck_first_z = 0;
		int deck_second_z = 0;
		int first_rail_z = 0;
		int second_rail_z = 0;
		int rail_y = 0;
		if (!detail::TryCenteredBlockSpan(center_z, half_width, deck_first_z, deck_second_z)
			|| !detail::TryBlockCoordinate(detail::WidenBlockCoordinate(deck_first_z) - 1, first_rail_z)
			|| !detail::TryBlockCoordinate(detail::WidenBlockCoordinate(deck_second_z) + 1, second_rail_z)
			|| !detail::TryBlockCoordinate(detail::WidenBlockCoordinate(block_y) + 1, rail_y))
		{
			return *this;
		}
		return FillBox(start_x, block_y, deck_first_z, end_x, block_y, deck_second_z, deck)
			.FillBox(start_x, rail_y, first_rail_z, end_x, rail_y, first_rail_z, rail)
			.FillBox(start_x, rail_y, second_rail_z, end_x, rail_y, second_rail_z, rail);
	}

	WorldConfig& WorldConfig::AddBridgeZ(
		int center_x, int block_y, int start_z, int end_z,
		int half_width, Block deck, Block rail)
	{
		int deck_first_x = 0;
		int deck_second_x = 0;
		int first_rail_x = 0;
		int second_rail_x = 0;
		int rail_y = 0;
		if (!detail::TryCenteredBlockSpan(center_x, half_width, deck_first_x, deck_second_x)
			|| !detail::TryBlockCoordinate(detail::WidenBlockCoordinate(deck_first_x) - 1, first_rail_x)
			|| !detail::TryBlockCoordinate(detail::WidenBlockCoordinate(deck_second_x) + 1, second_rail_x)
			|| !detail::TryBlockCoordinate(detail::WidenBlockCoordinate(block_y) + 1, rail_y))
		{
			return *this;
		}
		return FillBox(deck_first_x, block_y, start_z, deck_second_x, block_y, end_z, deck)
			.FillBox(first_rail_x, rail_y, start_z, first_rail_x, rail_y, end_z, rail)
			.FillBox(second_rail_x, rail_y, start_z, second_rail_x, rail_y, end_z, rail);
	}
}
