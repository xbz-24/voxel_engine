		return WorldBlockEdit{
			WorldBlockEdit::Kind::FillBox,
			std::min(first_corner_block_x, second_corner_block_x),
			std::min(first_corner_block_y, second_corner_block_y),
			std::min(first_corner_block_z, second_corner_block_z),
			std::max(first_corner_block_x, second_corner_block_x),
			std::max(first_corner_block_y, second_corner_block_y),
			std::max(first_corner_block_z, second_corner_block_z),
			block_id
		};
	}

	bool ApplyWorldBlockEdit(World& world, const WorldBlockEdit& world_block_edit)
	{
		switch (world_block_edit.kind)
		{
		case WorldBlockEdit::Kind::SetBlock:
			return world.SetBlock(
				world_block_edit.minimum_block_x,
				world_block_edit.minimum_block_y,
				world_block_edit.minimum_block_z,
				world_block_edit.block);
		case WorldBlockEdit::Kind::FillBox:
			return ApplyBlockBoxEdit(world, world_block_edit);
		default:
			return false;
		}
	}
}
