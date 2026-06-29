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

	WorldConfig& WorldConfig::SetBlock(int block_x, int block_y, int block_z, Block block)
	{
		return SetBlock(At(block_x, block_y, block_z), block);
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

	WorldConfig& WorldConfig::FillBox(int first_x, int first_y, int first_z, int second_x, int second_y, int second_z, Block block)
	{
		return FillBox(At(first_x, first_y, first_z), At(second_x, second_y, second_z), block);
	}

	WorldConfig& WorldConfig::ClearBox(int first_x, int first_y, int first_z, int second_x, int second_y, int second_z)
	{
		return FillBox(first_x, first_y, first_z, second_x, second_y, second_z, Air);
	}
