TEST_CASE("public world api normalizes block boxes")
{
	const voxel::BlockBox box = voxel::BlockBox::Between({ 4, 9, 2 }, { -1, 3, 8 });

	CHECK(box.minimum.x == -1);
	CHECK(box.minimum.y == 3);
	CHECK(box.minimum.z == 2);
	CHECK(box.maximum.x == 4);
	CHECK(box.maximum.y == 9);
	CHECK(box.maximum.z == 8);
}

TEST_CASE("public world scene helpers compose reusable objects")
{
	const voxel::WorldConfig world = voxel::World()
		.AddTree(0, 64, 0)
		.AddTree(8, 64, 8, voxel::BirchLog, voxel::BirchLeaves);

	REQUIRE(world.edits.size() == 6);
	CHECK(world.edits[0].block == voxel::OakLog);
	CHECK(world.edits[1].block == voxel::OakLeaves);
	CHECK(world.edits[3].block == voxel::BirchLog);
	CHECK(world.edits[4].block == voxel::BirchLeaves);
}

TEST_CASE("public world terrain settings are fluent")
{
	const voxel::TerrainPalette palette{
		voxel::Obsidian,
		voxel::Blackstone,
		voxel::Sandstone,
		voxel::RedSand,
		voxel::RedSandstone,
		voxel::Snow,
		voxel::CutSandstone,
		voxel::Gravel
	};
	const voxel::WorldConfig world = voxel::World(6)
		.WithTerrainSeed(1234U)
		.WithBaseSurfaceHeight(22)
		.WithTerrainGenerator(voxel::TerrainGenerator::Flat)
		.WithTerrainBiome(voxel::TerrainBiome::Desert)
		.WithTerrainPalette(palette);

	CHECK(world.size_chunks == 6);
	CHECK(world.terrain_seed == 1234U);
	CHECK(world.base_surface_height == 22);
	CHECK(world.terrain_generator == voxel::TerrainGenerator::Flat);
	CHECK(world.terrain_biome == voxel::TerrainBiome::Desert);
	CHECK(world.terrain_palette.surface == voxel::RedSand);
	CHECK(world.terrain_palette.subsurface == voxel::RedSandstone);
}

TEST_CASE("public scene api exposes semantic objects")
{
	const voxel::WorldConfig scene = voxel::Scene()
		.AddHouse(0, 64, 0)
		.AddLightPost(6, 64, -4)
		.AddPathZ(0, 63, -12, -4)
		.ClearBox(-1, 65, -4, 1, 66, -4);

	REQUIRE(scene.edits.size() == 13);
	CHECK(scene.edits[0].block == voxel::OakPlanks);
	CHECK(scene.edits[5].block == voxel::Bricks);
	CHECK(scene.edits[6].block == voxel::Air);
	CHECK(scene.edits[9].block == voxel::OakLog);
	CHECK(scene.edits[10].block == voxel::SeaLantern);
	CHECK(scene.edits[11].block == voxel::Gravel);
}

TEST_CASE("public scene api builds refined outdoor details")
{
	const voxel::WorldConfig scene = voxel::Scene()
		.AddPond(0, 63, 0)
		.AddGarden(8, 63, 0)
		.AddBridgeX(-3, 3, 64, 0)
		.AddBridgeZ(0, 64, -3, 3);

	REQUIRE(scene.edits.size() == 13);
	CHECK(scene.edits[0].block == voxel::MossyCobblestone);
	CHECK(scene.edits[1].block == voxel::Water);
	CHECK(scene.edits[2].block == voxel::Air);
	CHECK(scene.edits[3].block == voxel::Dirt);
	CHECK(scene.edits[4].block == voxel::HayBlock);
	CHECK(scene.edits[7].block == voxel::OakPlanks);
	CHECK(scene.edits[8].block == voxel::OakLog);
	CHECK(scene.edits[10].block == voxel::OakPlanks);
}

TEST_CASE("public scene runner is linkable")
{
	using SceneRunner = int (*)(voxel::WorldConfig);

	SceneRunner runner = &voxel::RunScene;

	CHECK(runner != nullptr);
}
