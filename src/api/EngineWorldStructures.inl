	WorldConfig& WorldConfig::AddHouse(int x, int y, int z, int radius, Block walls, Block roof)
	{
		const int left = x - radius;
		const int right = x + radius;
		const int front = z - radius;
		const int back = z + radius;
		const int wall_top = y + 3;
		const int roof_y = y + 4;

		return AddBox(left, y, front, right, y, back, walls)
			.AddBox(left, y + 1, front, right, wall_top, front, walls)
			.AddBox(left, y + 1, back, right, wall_top, back, walls)
			.AddBox(left, y + 1, front, left, wall_top, back, walls)
			.AddBox(right, y + 1, front, right, wall_top, back, walls)
			.AddBox(left - 1, roof_y, front - 1, right + 1, roof_y, back + 1, roof)
			.ClearBox(x - 1, y + 1, front, x + 1, y + 2, front)
			.ClearBox(x - 1, y + 1, z - 1, x + 1, wall_top, z + 1)
			.AddLamp(x, wall_top, z);
	}

	WorldConfig& WorldConfig::AddPond(int x, int y, int z, int radius, Block water, Block rim)
	{
		return AddFloor(x, y, z, radius, rim)
			.FillBox(x - radius + 1, y, z - radius + 1, x + radius - 1, y, z + radius - 1, water)
			.ClearBox(x - radius + 1, y + 1, z - radius + 1, x + radius - 1, y + 3, z + radius - 1);
	}

	WorldConfig& WorldConfig::AddGarden(int x, int y, int z, int half_width, int half_depth, Block soil, Block crop)
	{
		return FillBox(x - half_width, y, z - half_depth, x + half_width, y, z + half_depth, soil)
			.FillBox(x - half_width, y + 1, z - half_depth, x + half_width, y + 1, z + half_depth, crop)
			.FillBox(x - half_width - 1, y, z - half_depth - 1, x + half_width + 1, y, z - half_depth - 1, OakLog)
			.FillBox(x - half_width - 1, y, z + half_depth + 1, x + half_width + 1, y, z + half_depth + 1, OakLog);
	}

	WorldConfig& WorldConfig::AddTree(int x, int y, int z, Block trunk, Block leaves)
	{
		return AddColumn(x, y, z, 5, trunk)
			.AddBox(x - 2, y + 3, z - 2, x + 2, y + 5, z + 2, leaves)
			.AddBox(x - 1, y + 6, z - 1, x + 1, y + 6, z + 1, leaves);
	}
}
