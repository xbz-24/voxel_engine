		void PlaceUnderstory(ve::world::World& world, const DemoBounds& bounds, int x, int y, int z, int seed)
		{
			const float variant = Hash01(x, z, seed);
			const BlockId flower = variant < 0.08f ? BlockId::DiamondOre : (variant < 0.16f ? BlockId::AmethystBlock : BlockId::OakLeaves);
			SetBlock(world, bounds, x, y + 1, z, variant > 0.54f ? BlockId::MossBlock : flower);
			if (variant > 0.30f) SetBlock(world, bounds, x + 1, y + 1, z, BlockId::OakLeaves);
			if (variant > 0.58f) SetBlock(world, bounds, x - 1, y + 1, z, BlockId::BirchLeaves);
			if (variant > 0.78f) SetBlock(world, bounds, x, y + 2, z, BlockId::MossBlock);
		}

		void PlaceCanopyTree(ve::world::World& world, const DemoBounds& bounds, int x, int y, int z, int seed)
		{
			const int height = 12 + static_cast<int>(Hash01(x, z, seed) * 12.0f);
			const BlockId trunk = Hash01(x, z, seed + 1) < 0.70f ? BlockId::SpruceLog : BlockId::OakLog;
			const float leaf_roll = Hash01(x, z, seed + 2);
			const BlockId leaves = leaf_roll < 0.58f ? BlockId::OakLeaves : (leaf_roll < 0.90f ? BlockId::BirchLeaves : BlockId::CherryLeaves);
			FillBox(world, bounds, x, y + 1, z, x, y + height, z, trunk);
			FillBox(world, bounds, x - 1, y + 1, z, x - 1, y + 4, z, trunk);
			FillBox(world, bounds, x + 1, y + 1, z, x + 1, y + 4, z, trunk);
			FillBox(world, bounds, x, y + 1, z - 1, x, y + 3, z - 1, trunk);
			FillEllipsoid(world, bounds, x, y + height + 2, z, 6, 4, 6, leaves);
			FillEllipsoid(world, bounds, x - 4, y + height, z + 2, 4, 3, 4, leaves);
			FillEllipsoid(world, bounds, x + 4, y + height - 1, z - 1, 4, 3, 4, leaves);
			if (Hash01(x, z, seed + 5) > 0.52f) FillBox(world, bounds, x - 2, y + height - 6, z + 3, x - 2, y + height, z + 3, BlockId::MossBlock);
			if (Hash01(x, z, seed + 7) > 0.58f) FillBox(world, bounds, x + 3, y + height - 7, z - 2, x + 3, y + height, z - 2, BlockId::MossBlock);
		}

		void PlaceEmergentTree(ve::world::World& world, const DemoBounds& bounds, int x, int y, int z, int seed)
		{
			const int height = 24 + static_cast<int>(Hash01(x, z, seed) * 16.0f);
			FillBox(world, bounds, x, y + 1, z, x, y + height, z, BlockId::SpruceLog);
			FillBox(world, bounds, x - 1, y + 1, z, x + 1, y + 6, z, BlockId::SpruceLog);
			FillBox(world, bounds, x, y + 1, z - 1, x, y + 6, z + 1, BlockId::SpruceLog);
			for (int level = 0; level < 5; ++level)
			{
				const int canopy_y = y + height - (level * 4);
				const int radius = std::max(3, 9 - level);
				FillDisc(world, bounds, x, canopy_y, z, radius, level % 2 == 0 ? BlockId::OakLeaves : BlockId::BirchLeaves, seed + level);
				FillDisc(world, bounds, x, canopy_y + 1, z, std::max(2, radius - 2), BlockId::MossBlock, seed + 20 + level);
			}
			if (Hash01(x, z, seed + 8) > 0.42f) FillBox(world, bounds, x - 4, y + height - 18, z + 2, x - 4, y + height - 2, z + 2, BlockId::MossBlock);
			if (Hash01(x, z, seed + 9) > 0.50f) FillBox(world, bounds, x + 5, y + height - 16, z - 3, x + 5, y + height - 1, z - 3, BlockId::MossBlock);
		}
