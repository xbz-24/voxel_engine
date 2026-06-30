namespace
{
	int HighestSolidBlockY(
		const ve::world::terrain::BlockStorage& generatedBlockStorage,
		int localBlockX,
		int localBlockZ)
	{
		for (int localBlockY = ve::world::terrain::ChunkHeight - 1; localBlockY >= 0; localBlockY--)
		{
			if (generatedBlockStorage[localBlockX][localBlockY][localBlockZ] != ve::blocks::BlockId::Air)
			{
				return localBlockY;
			}
		}
		return -1;
	}
}

TEST_CASE("terrain generation settings control base height and block palette")
{
	ve::world::TerrainGenerationSettings lowTerrainSettings;
	lowTerrainSettings.baseSurfaceHeight = 8;
	lowTerrainSettings.blockPalette.surfaceBlock = ve::blocks::BlockId::Sandstone;
	lowTerrainSettings.blockPalette.subsurfaceBlock = ve::blocks::BlockId::RedSandstone;
	lowTerrainSettings.blockPalette.bedrockBlock = ve::blocks::BlockId::Obsidian;

	ve::world::TerrainGenerationSettings highTerrainSettings = lowTerrainSettings;
	highTerrainSettings.baseSurfaceHeight = 24;

	ve::world::terrain::BlockStorage lowTerrainBlocks{};
	ve::world::terrain::BlockStorage highTerrainBlocks{};
	ve::world::terrain::GenerateChunkTerrain(0, 0, lowTerrainSettings, lowTerrainBlocks);
	ve::world::terrain::GenerateChunkTerrain(0, 0, highTerrainSettings, highTerrainBlocks);

	const int localBlockX = 1;
	const int localBlockZ = 1;
	const int lowTerrainSurfaceY = HighestSolidBlockY(lowTerrainBlocks, localBlockX, localBlockZ);
	const int highTerrainSurfaceY = HighestSolidBlockY(highTerrainBlocks, localBlockX, localBlockZ);

	CHECK(highTerrainSurfaceY > lowTerrainSurfaceY);
	REQUIRE(lowTerrainSurfaceY > 0);
	CHECK(lowTerrainBlocks[localBlockX][lowTerrainSurfaceY][localBlockZ] == ve::blocks::BlockId::Sandstone);
	CHECK(lowTerrainBlocks[localBlockX][lowTerrainSurfaceY - 1][localBlockZ] == ve::blocks::BlockId::RedSandstone);
	CHECK(lowTerrainBlocks[localBlockX][0][localBlockZ] == ve::blocks::BlockId::Obsidian);
}

TEST_CASE("desert terrain biome selects a dry default surface palette")
{
	ve::world::TerrainGenerationSettings desertTerrainSettings;
	desertTerrainSettings.baseSurfaceHeight = 8;
	desertTerrainSettings.spawnBiome = ve::world::TerrainSpawnBiome::Desert;

	ve::world::terrain::BlockStorage desertTerrainBlocks{};
	ve::world::terrain::GenerateChunkTerrain(0, 0, desertTerrainSettings, desertTerrainBlocks);

	const int localBlockX = 1;
	const int localBlockZ = 1;
	const int desertSurfaceY = HighestSolidBlockY(desertTerrainBlocks, localBlockX, localBlockZ);

	REQUIRE(desertSurfaceY >= 0);
	CHECK(desertTerrainBlocks[localBlockX][desertSurfaceY][localBlockZ] == ve::blocks::BlockId::Sand);
}
