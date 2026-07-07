	WorldConfig& WorldConfig::AddFloor(int block_y, int radius, Block block)
	{
		return AddFloor(0, block_y, 0, radius, block);
	}

	WorldConfig& WorldConfig::AddFloor(int center_x, int block_y, int center_z, int radius, Block block)
	{
		return FillBox(
			center_x - radius,
			block_y,
			center_z - radius,
			center_x + radius,
			block_y,
			center_z + radius,
			block);
	}

	WorldConfig& WorldConfig::AddPlatform(int block_y, int radius, Block block)
	{
		return AddFloor(block_y, radius, block);
	}

	WorldConfig& WorldConfig::AddBox(int first_x, int first_y, int first_z, int second_x, int second_y, int second_z, Block block)
	{
		return FillBox(first_x, first_y, first_z, second_x, second_y, second_z, block);
	}

	WorldConfig& WorldConfig::AddColumn(int block_x, int base_y, int block_z, int height, Block block)
	{
		if (height <= 0)
		{
			return *this;
		}
		return FillBox(block_x, base_y, block_z, block_x, base_y + height - 1, block_z, block);
	}

	WorldConfig& WorldConfig::AddLamp(int block_x, int block_y, int block_z, Block block)
	{
		return SetBlock(block_x, block_y, block_z, block);
	}

	WorldConfig& WorldConfig::AddLightPost(int block_x, int base_y, int block_z, int height, Block post, Block light)
	{
		if (height <= 0)
		{
			return AddLamp(block_x, base_y, block_z, light);
		}
		return AddColumn(block_x, base_y, block_z, height, post)
			.AddLamp(block_x, base_y + height, block_z, light);
	}

	WorldConfig& WorldConfig::AddPathX(int start_x, int end_x, int block_y, int center_z, int half_width, Block block)
	{
		return FillBox(start_x, block_y, center_z - half_width, end_x, block_y, center_z + half_width, block);
	}

	WorldConfig& WorldConfig::AddPathZ(int center_x, int block_y, int start_z, int end_z, int half_width, Block block)
	{
		return FillBox(center_x - half_width, block_y, start_z, center_x + half_width, block_y, end_z, block);
	}

	WorldConfig& WorldConfig::AddBridgeX(int start_x, int end_x, int block_y, int center_z, int half_width, Block deck, Block rail)
	{
		const int deck_min_z = center_z - half_width;
		const int deck_max_z = center_z + half_width;
		const int north_rail_z = deck_min_z - 1;
		const int south_rail_z = deck_max_z + 1;

		return FillBox(start_x, block_y, deck_min_z, end_x, block_y, deck_max_z, deck)
			.FillBox(start_x, block_y + 1, north_rail_z, end_x, block_y + 1, north_rail_z, rail)
			.FillBox(start_x, block_y + 1, south_rail_z, end_x, block_y + 1, south_rail_z, rail);
	}

	WorldConfig& WorldConfig::AddBridgeZ(int center_x, int block_y, int start_z, int end_z, int half_width, Block deck, Block rail)
	{
		const int deck_min_x = center_x - half_width;
		const int deck_max_x = center_x + half_width;
		const int west_rail_x = deck_min_x - 1;
		const int east_rail_x = deck_max_x + 1;

		return FillBox(deck_min_x, block_y, start_z, deck_max_x, block_y, end_z, deck)
			.FillBox(west_rail_x, block_y + 1, start_z, west_rail_x, block_y + 1, end_z, rail)
			.FillBox(east_rail_x, block_y + 1, start_z, east_rail_x, block_y + 1, end_z, rail);
	}
