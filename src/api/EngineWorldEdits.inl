	WorldConfig& WorldConfig::SetBlock(BlockPosition position, Block block)
	{
		edits.push_back(WorldEdit{
			WorldEdit::Kind::SetBlock,
			position,
			{},
			block
		});
		return *this;
	}

	WorldConfig& WorldConfig::SetBlock(int x, int y, int z, Block block)
	{
		return SetBlock(At(x, y, z), block);
	}

	WorldConfig& WorldConfig::FillBox(BlockBox box, Block block)
	{
		edits.push_back(WorldEdit{
			WorldEdit::Kind::FillBox,
			{},
			box,
			block
		});
		return *this;
	}

	WorldConfig& WorldConfig::FillBox(BlockPosition first, BlockPosition second, Block block)
	{
		return FillBox(Box(first, second), block);
	}

	WorldConfig& WorldConfig::FillBox(int x1, int y1, int z1, int x2, int y2, int z2, Block block)
	{
		return FillBox(At(x1, y1, z1), At(x2, y2, z2), block);
	}

	WorldConfig& WorldConfig::ClearBox(int x1, int y1, int z1, int x2, int y2, int z2)
	{
		return FillBox(x1, y1, z1, x2, y2, z2, Air);
	}
