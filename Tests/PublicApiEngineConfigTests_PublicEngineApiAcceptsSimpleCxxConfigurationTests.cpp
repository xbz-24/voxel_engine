#include <doctest/doctest.h>

#include <voxel/Voxel.h>

#include <algorithm>
#include <chrono>
#include <filesystem>
#include <string>
#include <system_error>

TEST_CASE("public engine api accepts simple cxx configuration")
{
	voxel::EngineConfig config = voxel::EngineConfig::Default()
		.WithWindow(voxel::Window("API Smoke", 1024, 576).EnableVSync())
		.WithWorld(voxel::World()
			.WithSizeChunks(6)
			.SetBlock(0, 64, 0, voxel::Stone)
			.FillBox(-2, 63, -2, 2, 63, 2, voxel::Grass)
			.AddFloor(60, 4, voxel::Dirt)
			.AddPlatform(61, 2, voxel::OakPlanks)
			.AddColumn(3, 62, 3, 3, voxel::OakLog)
			.AddLamp(3, 65, 3))
		.WithCamera({ 20.0f, 70.0f, 20.0f }, { 0.0f, 64.0f, 0.0f })
		.UseVulkan()
		.WithRenderDistanceChunks(3);

	voxel::Engine engine{ config };
	engine.RequestStop();

	CHECK(config.window.title == "API Smoke");
	CHECK(config.window.width == 1024);
	CHECK(config.window.height == 576);
	CHECK(config.window.vsync);
	CHECK(config.world.size_chunks == 6);
	CHECK(config.camera.has_custom_view);
	CHECK(config.camera.position.y == doctest::Approx(70.0f));
	REQUIRE(config.world.edits.size() == 6);
	CHECK(config.world.edits[0].block == voxel::Stone);
	CHECK(config.world.edits[1].kind == voxel::WorldEdit::Kind::FillBox);
	CHECK(config.world.edits[2].box.minimum.x == -4);
	CHECK(config.world.edits[2].box.maximum.z == 4);
	CHECK(config.world.edits[4].box.maximum.y == 64);
	CHECK(config.world.edits[5].block == voxel::SeaLantern);
}

