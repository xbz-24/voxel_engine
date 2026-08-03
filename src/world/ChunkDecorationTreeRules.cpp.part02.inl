			{ 43, 10, 47, 14, BlockId::BirchLog, BlockId::BirchLeaves },
		} };

		for (const TreeDecorationRule& rule : tree_rules)
		{
			if (TryAddTreePattern(local_block_x, local_block_z, surface_y, world_block_x, world_block_z, rule, blocks))
			{
				return true;
			}
		}
		return false;
	}
}
