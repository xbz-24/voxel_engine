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
		{}
	}));
}
