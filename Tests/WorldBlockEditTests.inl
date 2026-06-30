TEST_CASE("world block edits normalize ranges and reuse single block apply path")
{
	ve::world::World world(ve::world::CreateInfoForSquareWorld(1));
	world.SpawnEmptyGrid(ve::world::FlatWorldSpawnSettings{ 1 });

	const ve::world::WorldBlockEdit reversed_box_edit = ve::world::MakeBlockBoxEdit(
		2,
		3,
		2,
		1,
		2,
		1,
		ve::blocks::BlockId::Stone);

	CHECK(ve::world::ApplyWorldBlockEdit(world, reversed_box_edit));
	CHECK(world.GetBlock(1, 2, 1) == ve::blocks::BlockId::Stone);
	CHECK(world.GetBlock(2, 3, 2) == ve::blocks::BlockId::Stone);

	const ve::world::WorldBlockEdit single_block_edit = ve::world::MakeSingleBlockEdit(
		1,
		2,
		1,
		ve::blocks::BlockId::Air);

	CHECK(ve::world::ApplyWorldBlockEdit(world, single_block_edit));
	CHECK(world.GetBlock(1, 2, 1) == ve::blocks::BlockId::Air);

	ve::world::WorldBlockEdit invalid_kind_edit = ve::world::MakeSingleBlockEdit(
		1,
		2,
		1,
		ve::blocks::BlockId::Stone);
	invalid_kind_edit.kind = static_cast<ve::world::WorldBlockEdit::Kind>(255);

	CHECK(!ve::world::ApplyWorldBlockEdit(world, invalid_kind_edit));
	CHECK(world.GetBlock(1, 2, 1) == ve::blocks::BlockId::Air);
}
