	WorldConfig& WorldConfig::AddHouse(int center_x, int base_y, int center_z, int radius, Block walls, Block roof)
	{
		const int left = center_x - radius;
		const int right = center_x + radius;
		const int front = center_z - radius;
		const int back = center_z + radius;
		const int wall_top = base_y + 3;
		const int roof_y = base_y + 4;

		return AddBox(left, base_y, front, right, base_y, back, walls)
			.AddBox(left, base_y + 1, front, right, wall_top, front, walls)
			.AddBox(left, base_y + 1, back, right, wall_top, back, walls)
			.AddBox(left, base_y + 1, front, left, wall_top, back, walls)
			.AddBox(right, base_y + 1, front, right, wall_top, back, walls)
			.AddBox(left - 1, roof_y, front - 1, right + 1, roof_y, back + 1, roof)
			.ClearBox(center_x - 1, base_y + 1, front, center_x + 1, base_y + 2, front)
			.ClearBox(center_x - 1, base_y + 1, center_z - 1, center_x + 1, wall_top, center_z + 1)
			.AddLamp(center_x, wall_top, center_z);
	}

	WorldConfig& WorldConfig::AddPond(int center_x, int block_y, int center_z, int radius, Block water, Block rim)
	{
		const int inner_left = center_x - radius + 1;
		const int inner_right = center_x + radius - 1;
		const int inner_front = center_z - radius + 1;
		const int inner_back = center_z + radius - 1;

		return AddFloor(center_x, block_y, center_z, radius, rim)
			.FillBox(inner_left, block_y, inner_front, inner_right, block_y, inner_back, water)
			.ClearBox(inner_left, block_y + 1, inner_front, inner_right, block_y + 3, inner_back);
	}

	WorldConfig& WorldConfig::AddGarden(int center_x, int block_y, int center_z, int half_width, int half_depth, Block soil, Block crop)
	{
		const int plot_left = center_x - half_width;
		const int plot_right = center_x + half_width;
		const int plot_front = center_z - half_depth;
		const int plot_back = center_z + half_depth;
		const int fence_left = plot_left - 1;
		const int fence_right = plot_right + 1;

		return FillBox(plot_left, block_y, plot_front, plot_right, block_y, plot_back, soil)
			.FillBox(plot_left, block_y + 1, plot_front, plot_right, block_y + 1, plot_back, crop)
			.FillBox(fence_left, block_y, plot_front - 1, fence_right, block_y, plot_front - 1, OakLog)
			.FillBox(fence_left, block_y, plot_back + 1, fence_right, block_y, plot_back + 1, OakLog);
	}

	WorldConfig& WorldConfig::AddTree(int trunk_x, int base_y, int trunk_z, Block trunk, Block leaves)
	{
		const int lower_canopy_y = base_y + 3;
		const int upper_canopy_y = base_y + 5;
		const int crown_y = base_y + 6;

		return AddColumn(trunk_x, base_y, trunk_z, 5, trunk)
			.AddBox(trunk_x - 2, lower_canopy_y, trunk_z - 2, trunk_x + 2, upper_canopy_y, trunk_z + 2, leaves)
			.AddBox(trunk_x - 1, crown_y, trunk_z - 1, trunk_x + 1, crown_y, trunk_z + 1, leaves);
	}
}
