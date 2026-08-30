#include "PublicWorldGeometryOverflowTestSupport.h"

namespace geometry = public_world_geometry_tests;

TEST_CASE("public structure helpers reject derived coordinate overflow atomically")
{
	SUBCASE("house X")
	{
		geometry::CheckComposedNoOp([](voxel::WorldConfig& world)
		{
			world.AddHouse(geometry::kIntMaximum, 0, 0, 1);
		});
	}
	SUBCASE("house Y")
	{
		geometry::CheckComposedNoOp([](voxel::WorldConfig& world)
		{
			world.AddHouse(0, geometry::kIntMaximum, 0, 1);
		});
	}
	SUBCASE("pond X")
	{
		geometry::CheckComposedNoOp([](voxel::WorldConfig& world)
		{
			world.AddPond(geometry::kIntMaximum, 0, 0, 1);
		});
	}
	SUBCASE("pond Y")
	{
		geometry::CheckComposedNoOp([](voxel::WorldConfig& world)
		{
			world.AddPond(0, geometry::kIntMaximum, 0, 1);
		});
	}
	SUBCASE("garden X")
	{
		geometry::CheckComposedNoOp([](voxel::WorldConfig& world)
		{
			world.AddGarden(geometry::kIntMaximum, 0, 0, 1, 1);
		});
	}
	SUBCASE("garden Y")
	{
		geometry::CheckComposedNoOp([](voxel::WorldConfig& world)
		{
			world.AddGarden(0, geometry::kIntMaximum, 0, 1, 1);
		});
	}
	SUBCASE("tree X")
	{
		geometry::CheckComposedNoOp([](voxel::WorldConfig& world)
		{
			world.AddTree(geometry::kIntMinimum, 0, 0);
		});
	}
	SUBCASE("tree Y")
	{
		geometry::CheckComposedNoOp([](voxel::WorldConfig& world)
		{
			world.AddTree(0, geometry::kIntMaximum, 0);
		});
	}
}
