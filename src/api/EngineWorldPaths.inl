	WorldConfig& WorldConfig::AddFloor(int y, int radius, Block block)
	{
		return AddFloor(0, y, 0, radius, block);
	}

	WorldConfig& WorldConfig::AddFloor(int center_x, int y, int center_z, int radius, Block block)
	{
		return FillBox(
			center_x - radius,
			y,
			center_z - radius,
			center_x + radius,
			y,
			center_z + radius,
			block);
	}

	WorldConfig& WorldConfig::AddPlatform(int y, int radius, Block block)
	{
		return AddFloor(y, radius, block);
	}

	WorldConfig& WorldConfig::AddBox(int x1, int y1, int z1, int x2, int y2, int z2, Block block)
	{
		return FillBox(x1, y1, z1, x2, y2, z2, block);
	}

	WorldConfig& WorldConfig::AddColumn(int x, int y, int z, int height, Block block)
	{
		if (height <= 0)
		{
			return *this;
		}
		return FillBox(x, y, z, x, y + height - 1, z, block);
	}

	WorldConfig& WorldConfig::AddLamp(int x, int y, int z, Block block)
	{
		return SetBlock(x, y, z, block);
	}

	WorldConfig& WorldConfig::AddLightPost(int x, int y, int z, int height, Block post, Block light)
	{
		if (height <= 0)
		{
			return AddLamp(x, y, z, light);
		}
		return AddColumn(x, y, z, height, post)
			.AddLamp(x, y + height, z, light);
	}

	WorldConfig& WorldConfig::AddPathX(int x1, int x2, int y, int z, int half_width, Block block)
	{
		return FillBox(x1, y, z - half_width, x2, y, z + half_width, block);
	}

	WorldConfig& WorldConfig::AddPathZ(int x, int y, int z1, int z2, int half_width, Block block)
	{
		return FillBox(x - half_width, y, z1, x + half_width, y, z2, block);
	}

	WorldConfig& WorldConfig::AddBridgeX(int x1, int x2, int y, int z, int half_width, Block deck, Block rail)
	{
		return FillBox(x1, y, z - half_width, x2, y, z + half_width, deck)
			.FillBox(x1, y + 1, z - half_width - 1, x2, y + 1, z - half_width - 1, rail)
			.FillBox(x1, y + 1, z + half_width + 1, x2, y + 1, z + half_width + 1, rail);
	}

	WorldConfig& WorldConfig::AddBridgeZ(int x, int y, int z1, int z2, int half_width, Block deck, Block rail)
	{
		return FillBox(x - half_width, y, z1, x + half_width, y, z2, deck)
			.FillBox(x - half_width - 1, y + 1, z1, x - half_width - 1, y + 1, z2, rail)
			.FillBox(x + half_width + 1, y + 1, z1, x + half_width + 1, y + 1, z2, rail);
	}
