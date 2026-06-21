#include <doctest/doctest.h>

#include <voxel/Voxel.h>

TEST_CASE("public engine api accepts simple cxx configuration")
{
	voxel::EngineConfig config = voxel::EngineConfig::MinecraftDemo()
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

TEST_CASE("public run helper is available without exposing internals")
{
	const voxel::EngineConfig config = voxel::EngineConfig::Default()
		.WithWindow("Window API Smoke", 800, 600)
		.UseOpenGLCompatibility()
		.HideDebugOverlay();

	CHECK(!config.window.fullscreen);
	CHECK(config.window.title == "Window API Smoke");
	CHECK(config.window.width == 800);
	CHECK(config.window.height == 600);
	CHECK(config.graphics_api == voxel::GraphicsApi::OpenGLCompatibility);
	CHECK(!config.show_debug_overlay);
}

TEST_CASE("public top-level helpers expose the shortest startup path")
{
	const voxel::EngineConfig config = voxel::MinecraftDemo();

	CHECK(config.graphics_api == voxel::GraphicsApi::Vulkan);
	CHECK(config.world.size_chunks == 8);
	CHECK(config.window.title == "Voxel Engine");
}

TEST_CASE("public demo presets are ready to run")
{
	const voxel::EngineConfig world_edit = voxel::WorldEditDemo();
	const voxel::EngineConfig showcase = voxel::EngineConfig::ShowcaseDemo();

	CHECK(world_edit.window.title == "Voxel API - World Edit");
	CHECK(world_edit.world.edits.size() == 4);
	CHECK(world_edit.camera.has_custom_view);
	CHECK(showcase.window.title == "Voxel API - Showcase");
	CHECK(showcase.world.edits.size() == 9);
	CHECK(!showcase.show_debug_overlay);
}

TEST_CASE("public one-call demo runners are linkable")
{
	using Runner = int (*)();

	Runner minecraft = &voxel::RunMinecraftDemo;
	Runner world_edit = &voxel::RunWorldEditDemo;
	Runner showcase = &voxel::RunShowcaseDemo;

	CHECK(minecraft != nullptr);
	CHECK(world_edit != nullptr);
	CHECK(showcase != nullptr);
}

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

TEST_CASE("public scene runner is linkable")
{
	using SceneRunner = int (*)(voxel::WorldConfig);

	SceneRunner runner = &voxel::RunScene;

	CHECK(runner != nullptr);
}
