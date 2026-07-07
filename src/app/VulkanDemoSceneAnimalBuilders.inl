		void PlaceFrog(ve::world::World& world, const DemoBounds& bounds, int x, int y, int z)
		{
			FillBox(world, bounds, x - 1, y + 1, z, x + 1, y + 1, z + 1, BlockId::MossBlock);
			SetBlock(world, bounds, x - 1, y + 2, z, BlockId::Blackstone);
			SetBlock(world, bounds, x + 1, y + 2, z, BlockId::Blackstone);
			SetBlock(world, bounds, x, y + 2, z + 1, BlockId::BirchLeaves);
		}

		void PlaceBird(ve::world::World& world, const DemoBounds& bounds, int x, int y, int z)
		{
			SetBlock(world, bounds, x, y + 6, z, BlockId::DiamondOre);
			FillBox(world, bounds, x - 2, y + 6, z, x - 1, y + 6, z, BlockId::AmethystBlock);
			FillBox(world, bounds, x + 1, y + 6, z, x + 2, y + 6, z, BlockId::AmethystBlock);
			SetBlock(world, bounds, x, y + 6, z - 1, BlockId::GoldOre);
			SetBlock(world, bounds, x, y + 5, z, BlockId::OakLog);
		}

		void PlaceTapir(ve::world::World& world, const DemoBounds& bounds, int x, int y, int z)
		{
			FillBox(world, bounds, x - 1, y + 1, z, x + 1, y + 2, z + 2, BlockId::Blackstone);
			FillBox(world, bounds, x - 1, y + 1, z + 3, x + 1, y + 2, z + 3, BlockId::Snow);
			SetBlock(world, bounds, x, y + 3, z + 3, BlockId::Diorite);
		}

		void PlaceCapybara(ve::world::World& world, const DemoBounds& bounds, int x, int y, int z)
		{
			FillBox(world, bounds, x - 2, y + 1, z, x + 2, y + 2, z + 2, BlockId::OakPlanks);
			FillBox(world, bounds, x - 1, y + 2, z + 3, x + 1, y + 3, z + 3, BlockId::OakLog);
			FillBox(world, bounds, x - 2, y + 1, z, x + 2, y + 1, z, BlockId::Blackstone);
			SetBlock(world, bounds, x - 1, y + 3, z + 4, BlockId::Blackstone);
			SetBlock(world, bounds, x + 1, y + 3, z + 4, BlockId::Blackstone);
		}

		void PlaceDeer(ve::world::World& world, const DemoBounds& bounds, int x, int y, int z)
		{
			FillBox(world, bounds, x - 1, y + 3, z, x + 1, y + 4, z + 3, BlockId::HayBlock);
			FillBox(world, bounds, x - 1, y + 1, z, x - 1, y + 2, z, BlockId::OakLog);
			FillBox(world, bounds, x + 1, y + 1, z, x + 1, y + 2, z, BlockId::OakLog);
			FillBox(world, bounds, x - 1, y + 1, z + 3, x - 1, y + 2, z + 3, BlockId::OakLog);
			FillBox(world, bounds, x + 1, y + 1, z + 3, x + 1, y + 2, z + 3, BlockId::OakLog);
			SetBlock(world, bounds, x, y + 5, z + 4, BlockId::OakLog);
			SetBlock(world, bounds, x - 1, y + 6, z + 4, BlockId::BirchLog);
			SetBlock(world, bounds, x + 1, y + 6, z + 4, BlockId::BirchLog);
		}

		void ClearAnimalSpace(ve::world::World& world, const DemoBounds& bounds, int x, int y, int z)
		{
			FillBox(world, bounds, x - 3, y + 1, z - 3, x + 3, y + 8, z + 5, BlockId::Air);
		}
