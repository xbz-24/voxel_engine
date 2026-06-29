		void BuildAquaFallback(ve::world::World& world, const DemoBounds& bounds, int base_y)
		{
			const int x = bounds.center_x;
			const int z = bounds.center_z;
			FillEllipsoid(world, bounds, x, base_y + 30, z, 10, 18, 6, BlockId::Snow);
			FillEllipsoid(world, bounds, x, base_y + 55, z, 8, 10, 7, BlockId::Sandstone);
			FillEllipsoid(world, bounds, x, base_y + 60, z, 12, 9, 10, BlockId::DiamondOre);
			FillEllipsoid(world, bounds, x - 8, base_y + 47, z, 5, 18, 4, BlockId::DiamondOre);
			FillEllipsoid(world, bounds, x + 8, base_y + 47, z, 5, 18, 4, BlockId::DiamondOre);
			FillBox(world, bounds, x - 6, base_y + 12, z - 2, x - 2, base_y + 29, z + 2, BlockId::Sandstone);
			FillBox(world, bounds, x + 2, base_y + 12, z - 2, x + 6, base_y + 29, z + 2, BlockId::Sandstone);
			FillBox(world, bounds, x - 8, base_y, z - 3, x - 3, base_y + 12, z + 3, BlockId::DiamondOre);
			FillBox(world, bounds, x + 3, base_y, z - 3, x + 8, base_y + 12, z + 3, BlockId::DiamondOre);
			FillBox(world, bounds, x - 20, base_y + 36, z, x - 11, base_y + 40, z + 3, BlockId::Sandstone);
			FillBox(world, bounds, x + 11, base_y + 36, z, x + 20, base_y + 40, z + 3, BlockId::Sandstone);
			FillBox(world, bounds, x + 23, base_y + 4, z + 6, x + 25, base_y + 62, z + 8, BlockId::OakLog);
			FillEllipsoid(world, bounds, x + 24, base_y + 66, z + 7, 5, 5, 5, BlockId::SeaLantern);
		}

		void BuildSponzaFallback(ve::world::World& world, const DemoBounds& bounds, int base_y)
		{
			const int cx = bounds.center_x;
			const int cz = bounds.center_z;
			FillBox(world, bounds, cx - 58, base_y, cz - 34, cx + 58, base_y, cz + 34, BlockId::Diorite);
			for (int side = -1; side <= 1; side += 2)
			{
				const int z = cz + (side * 30);
				FillBox(world, bounds, cx - 62, base_y + 1, z - 3, cx + 62, base_y + 4, z + 3, BlockId::Terracotta);
				for (int i = -5; i <= 5; ++i)
				{
					const int x = cx + (i * 11);
					FillBox(world, bounds, x - 2, base_y + 1, z - 2, x + 2, base_y + 34, z + 2, BlockId::Diorite);
					FillEllipsoid(world, bounds, x, base_y + 38, z, 5, 4, 4, BlockId::CutSandstone);
					if (i < 5)
					{
						FillBox(world, bounds, x + 4, base_y + 29, z - 2, x + 7, base_y + 34, z + 2, BlockId::CutSandstone);
					}
				}
			}
			FillBox(world, bounds, cx - 64, base_y + 40, cz - 36, cx + 64, base_y + 44, cz + 36, BlockId::RedSandstone);
			FillBox(world, bounds, cx - 50, base_y + 45, cz - 30, cx + 50, base_y + 48, cz + 30, BlockId::Terracotta);
			for (int x = cx - 44; x <= cx + 44; x += 12)
			{
				FillBox(world, bounds, x - 2, base_y + 1, cz - 6, x + 2, base_y + 9, cz + 6, BlockId::MossBlock);
			}
		}
