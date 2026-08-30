#include "PublicWorldGeometryOverflowTestSupport.h"

namespace geometry = public_world_geometry_tests;

TEST_CASE("direct public fill boxes retain the full integer range")
{
	auto world = geometry::SentinelWorld();
	world.FillBox(
		geometry::kIntMinimum, geometry::kIntMinimum, geometry::kIntMinimum,
		geometry::kIntMaximum, geometry::kIntMaximum, geometry::kIntMaximum,
		voxel::Stone);

	REQUIRE(world.edits.size() == 2U);
	geometry::CheckFillEdit(world.edits[1],
		voxel::At(geometry::kIntMinimum, geometry::kIntMinimum, geometry::kIntMinimum),
		voxel::At(geometry::kIntMaximum, geometry::kIntMaximum, geometry::kIntMaximum),
		voxel::Stone);
}

TEST_CASE("public columns accept their last valid height and reject overflow")
{
	SUBCASE("one block at the maximum height")
	{
		auto world = geometry::SentinelWorld();
		world.AddColumn(-7, geometry::kIntMaximum, 9, 1, voxel::OakLog);

		REQUIRE(world.edits.size() == 2U);
		geometry::CheckFillEdit(world.edits[1],
			voxel::At(-7, geometry::kIntMaximum, 9),
			voxel::At(-7, geometry::kIntMaximum, 9), voxel::OakLog);
	}

	SUBCASE("two blocks above the maximum height")
	{
		geometry::CheckComposedNoOp([](voxel::WorldConfig& world)
		{
			world.AddColumn(-7, geometry::kIntMaximum, 9, 2, voxel::OakLog);
		});
	}
}

TEST_CASE("public floor helpers reject derived coordinate overflow")
{
	SUBCASE("offset floor")
	{
		geometry::CheckComposedNoOp([](voxel::WorldConfig& world)
		{
			world.AddFloor(geometry::kIntMaximum, 4, 0, 1, voxel::Stone);
		});
	}

	SUBCASE("centered platform")
	{
		geometry::CheckComposedNoOp([](voxel::WorldConfig& world)
		{
			world.AddPlatform(4, geometry::kIntMinimum, voxel::Stone);
		});
	}
}

TEST_CASE("public light posts validate the light before adding their column")
{
	SUBCASE("overflowing light")
	{
		geometry::CheckComposedNoOp([](voxel::WorldConfig& world)
		{
			world.AddLightPost(4, geometry::kIntMaximum, -5, 1);
		});
	}

	SUBCASE("zero height remains a lamp")
	{
		auto world = geometry::SentinelWorld();
		world.AddLightPost(4, geometry::kIntMaximum, -5, 0);

		REQUIRE(world.edits.size() == 2U);
		geometry::CheckSetEdit(world.edits[1],
			voxel::At(4, geometry::kIntMaximum, -5), voxel::SeaLantern);
	}
}
