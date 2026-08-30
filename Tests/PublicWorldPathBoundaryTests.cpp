#include "PublicWorldGeometryOverflowTestSupport.h"

namespace geometry = public_world_geometry_tests;

TEST_CASE("public path helpers reject derived coordinate overflow")
{
	SUBCASE("path along X")
	{
		geometry::CheckComposedNoOp([](voxel::WorldConfig& world)
		{
			world.AddPathX(-2, 2, 4, geometry::kIntMaximum, 1);
		});
	}

	SUBCASE("path along Z")
	{
		geometry::CheckComposedNoOp([](voxel::WorldConfig& world)
		{
			world.AddPathZ(geometry::kIntMinimum, 4, -2, 2, 1);
		});
	}
}

TEST_CASE("public bridge helpers reject center and height overflow atomically")
{
	SUBCASE("bridge X center")
	{
		geometry::CheckComposedNoOp([](voxel::WorldConfig& world)
		{
			world.AddBridgeX(-2, 2, 4, geometry::kIntMaximum, 1);
		});
	}

	SUBCASE("bridge X height")
	{
		geometry::CheckComposedNoOp([](voxel::WorldConfig& world)
		{
			world.AddBridgeX(-2, 2, geometry::kIntMaximum, 0, 1);
		});
	}

	SUBCASE("bridge Z center")
	{
		geometry::CheckComposedNoOp([](voxel::WorldConfig& world)
		{
			world.AddBridgeZ(geometry::kIntMinimum, 4, -2, 2, 1);
		});
	}

	SUBCASE("bridge Z height")
	{
		geometry::CheckComposedNoOp([](voxel::WorldConfig& world)
		{
			world.AddBridgeZ(0, geometry::kIntMaximum, -2, 2, 1);
		});
	}
}

TEST_CASE("a public bridge can reach the integer boundary exactly")
{
	auto world = geometry::SentinelWorld();
	world.AddBridgeX(-2, 2, geometry::kIntMaximum - 1,
		geometry::kIntMaximum - 2, 1);

	REQUIRE(world.edits.size() == 4U);
	geometry::CheckFillEdit(world.edits[1],
		voxel::At(-2, geometry::kIntMaximum - 1, geometry::kIntMaximum - 3),
		voxel::At(2, geometry::kIntMaximum - 1, geometry::kIntMaximum - 1),
		voxel::OakPlanks);
	geometry::CheckFillEdit(world.edits[2],
		voxel::At(-2, geometry::kIntMaximum, geometry::kIntMaximum - 4),
		voxel::At(2, geometry::kIntMaximum, geometry::kIntMaximum - 4),
		voxel::OakLog);
	geometry::CheckFillEdit(world.edits[3],
		voxel::At(-2, geometry::kIntMaximum, geometry::kIntMaximum),
		voxel::At(2, geometry::kIntMaximum, geometry::kIntMaximum), voxel::OakLog);
}
