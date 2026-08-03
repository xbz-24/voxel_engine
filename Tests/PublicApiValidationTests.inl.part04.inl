	CHECK(loaded.size_chunks == 5);
	CHECK(loaded.terrain_seed == 77U);
	CHECK(loaded.base_surface_height == 23);
	CHECK(loaded.terrain_generator == voxel::TerrainGenerator::Flat);
	CHECK(loaded.terrain_biome == voxel::TerrainBiome::Alpine);
	CHECK(loaded.terrain_palette.deep_stone == voxel::Blackstone);
	CHECK(loaded.terrain_palette.gravel_patch == voxel::Andesite);
	REQUIRE(loaded.edits.size() == 2);
	CHECK(loaded.edits[0].kind == voxel::WorldEdit::Kind::SetBlock);
	CHECK(loaded.edits[0].position.x == 1);
	CHECK(loaded.edits[0].block == voxel::DiamondOre);
	CHECK(loaded.edits[1].kind == voxel::WorldEdit::Kind::FillBox);
	CHECK(loaded.edits[1].box.minimum.x == -1);
	CHECK(loaded.edits[1].block == voxel::Grass);
}
