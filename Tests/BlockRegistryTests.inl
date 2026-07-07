TEST_CASE("block registry stores block metadata by stable id")
{
	const ve::blocks::BlockRegistry registry(
		ve::assets::AssetPaths{},
		ve::blocks::BlockRegistry::TextureLoading::MetadataOnly);

	CHECK(registry.Contains(ve::blocks::BlockId::Stone));
	CHECK(!registry.Contains(ve::blocks::BlockId::Count));
	CHECK(registry.RegisteredBlockCount() == static_cast<std::size_t>(ve::blocks::BlockId::Count));
	CHECK(registry.Get(ve::blocks::BlockId::Count).id == ve::blocks::BlockId::Air);
}

TEST_CASE("block registry can replace metadata for a stable id")
{
	ve::blocks::BlockRegistry registry(
		ve::assets::AssetPaths{},
		ve::blocks::BlockRegistry::TextureLoading::MetadataOnly);
	ve::blocks::BlockType ghost_glass = registry.Get(ve::blocks::BlockId::Glass);
	ghost_glass.name = "Ghost Glass";
	ghost_glass.isSolid = true;

	CHECK(registry.Register(ghost_glass));
	CHECK(registry.Get(ve::blocks::BlockId::Glass).name == "Ghost Glass");
	CHECK(registry.IsSolid(ve::blocks::BlockId::Glass));
	CHECK(!registry.Register(ve::blocks::BlockType{
		ve::blocks::BlockId::Count,
		"Invalid",
		false,
		{},
		{},
		{}
	}));
}

TEST_CASE("block registry exposes gameplay extension metadata")
{
	ve::blocks::BlockRegistry registry(
		ve::assets::AssetPaths{},
		ve::blocks::BlockRegistry::TextureLoading::MetadataOnly);

	CHECK(registry.BlocksMovement(ve::blocks::BlockId::Stone));
	CHECK(registry.IsTransparent(ve::blocks::BlockId::Glass));
	REQUIRE(registry.GameplayFor(ve::blocks::BlockId::Stone).drops.size() == 1U);
	CHECK(registry.GameplayFor(ve::blocks::BlockId::Stone).drops.front().id == ve::blocks::BlockId::Stone);

	ve::blocks::BlockType water = registry.Get(ve::blocks::BlockId::Water);
	water.gameplay.collision = ve::blocks::BlockCollisionMode::None;
	water.gameplay.transparent = true;
	water.gameplay.footstep_sound = "water.step";
	water.gameplay.break_sound = "water.break";
	water.gameplay.drops.clear();
	CHECK(registry.Register(water));
	CHECK(!registry.BlocksMovement(ve::blocks::BlockId::Water));
	CHECK(registry.IsTransparent(ve::blocks::BlockId::Water));
	CHECK(registry.GameplayFor(ve::blocks::BlockId::Water).break_sound == "water.break");
}

TEST_CASE("block registry accepts data-authored block definitions")
{
	ve::blocks::BlockRegistry registry(
		ve::assets::AssetPaths{},
		ve::blocks::BlockRegistry::TextureLoading::MetadataOnly);

	ve::blocks::DataBlockDefinition authored_bricks{};
	authored_bricks.id = ve::blocks::BlockId::Bricks;
	authored_bricks.name = "Authored Bricks";
	authored_bricks.is_solid = true;
	authored_bricks.face_texture_files.fill("authored_bricks.png");

	CHECK(registry.RegisterDataDefinition(
		authored_bricks,
		ve::assets::AssetPaths{},
		ve::blocks::BlockRegistry::TextureLoading::MetadataOnly));
	CHECK(registry.Get(ve::blocks::BlockId::Bricks).name == "Authored Bricks");
	CHECK(registry.OccludesNeighborFaces(ve::blocks::BlockId::Bricks));
	CHECK(registry.GameplayFor(ve::blocks::BlockId::Bricks).drops.front().id == ve::blocks::BlockId::Bricks);
	CHECK(registry.TextureFor(ve::blocks::BlockId::Bricks, ve::blocks::BlockFace::Top) == ve::rendering::kInvalidTextureHandle);

	const ve::blocks::DataBlockDefinition invalid_count{
		ve::blocks::BlockId::Count,
		"Invalid",
		false,
		{},
		{},
		{}
	};
	const ve::blocks::DataBlockDefinition invalid_name{
		ve::blocks::BlockId::Stone,
		"",
		true,
		{},
		{},
		{}
	};
	const std::array definitions{ authored_bricks, invalid_count, invalid_name };

	CHECK(registry.RegisterDataDefinitions(
		definitions,
		ve::assets::AssetPaths{},
		ve::blocks::BlockRegistry::TextureLoading::MetadataOnly) == 1U);
}
