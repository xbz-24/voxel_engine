#include <doctest/doctest.h>

#include <voxel/Voxel.h>

#include <algorithm>
#include <filesystem>

// TODO: Add an installed-SDK compile test that includes only public headers and links without private source dirs.
// TODO: Add negative validation tests once EngineConfig exposes structured errors for invalid public inputs.
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
	const voxel::EngineConfig config = voxel::DesertDemo();
	const voxel::EngineConfig aqua = voxel::AquaModelDemo();
	const voxel::EngineConfig sponza = voxel::SponzaAtriumDemo();
	const voxel::EngineConfig legacy_alias = voxel::MinecraftDemo();
	const voxel::SdkVersion version = voxel::Version();
	const voxel::SdkFeatures features = voxel::Features();

	CHECK(config.graphics_api == voxel::GraphicsApi::Vulkan);
	CHECK(config.world.size_chunks == 20);
	CHECK(config.render_distance_chunks == 16);
	CHECK(config.window.title == "Voxel Engine - Crystal Jungle");
	CHECK(config.demo_scene == voxel::DemoScene::HyperrealDesert);
	CHECK(aqua.demo_scene == voxel::DemoScene::AquaModel);
	CHECK(sponza.demo_scene == voxel::DemoScene::SponzaAtrium);
	CHECK(aqua.window.title == "Voxel Engine - Aqua Model Preview");
	CHECK(sponza.window.title == "Voxel Engine - Sponza Atrium Preview");
	CHECK(legacy_alias.window.title == config.window.title);
	CHECK(version.major == 0);
	CHECK(features.vulkan_by_default);
	CHECK(features.world_config_serialization);
	CHECK(!features.scene_graph_authoring);
}

TEST_CASE("public demo presets are ready to run")
{
	const voxel::EngineConfig world_edit = voxel::WorldEditDemo();
	const voxel::EngineConfig showcase = voxel::EngineConfig::ShowcaseDemo();

	CHECK(world_edit.window.title == "Voxel API - World Edit");
	CHECK(world_edit.world.edits.size() == 30);
	CHECK(world_edit.camera.has_custom_view);
	CHECK(showcase.window.title == "Voxel API - Showcase");
	CHECK(showcase.world.edits.size() == 40);
	CHECK(!showcase.show_debug_overlay);
}

TEST_CASE("public arcade demo presets are ready to run")
{
	const voxel::EngineConfig snake = voxel::ArcadeSnakeDemo();
	const voxel::EngineConfig paddle = voxel::EngineConfig::ArcadePaddleDemo();
	const voxel::EngineConfig blocks = voxel::ArcadeBlocksDemo();
	const voxel::EngineConfig invaders = voxel::ArcadeInvadersDemo();
	const voxel::EngineConfig maze = voxel::ArcadeMazeDemo();

	CHECK(snake.window.title == "Voxel Arcade - Snake Trail");
	CHECK(paddle.window.title == "Voxel Arcade - Paddle Duel");
	CHECK(blocks.window.title == "Voxel Arcade - Falling Blocks");
	CHECK(invaders.window.title == "Voxel Arcade - Star Defenders");
	CHECK(maze.window.title == "Voxel Arcade - Neon Maze");
	CHECK(snake.graphics_api == voxel::GraphicsApi::Vulkan);
	CHECK(!maze.show_debug_overlay);
	CHECK(snake.camera.has_custom_view);
	CHECK(paddle.world.edits.size() > 20);
	CHECK(blocks.world.edits.size() > 20);
	CHECK(invaders.world.edits.size() > 50);
	CHECK(maze.world.edits.size() > 30);
}

TEST_CASE("public one-call demo runners are linkable")
{
	using Runner = int (*)();

	Runner desert = &voxel::RunDesertDemo;
	Runner aqua = &voxel::RunAquaModelDemo;
	Runner sponza = &voxel::RunSponzaAtriumDemo;
	Runner minecraft = &voxel::RunMinecraftDemo;
	Runner world_edit = &voxel::RunWorldEditDemo;
	Runner showcase = &voxel::RunShowcaseDemo;
	Runner arcade_snake = &voxel::RunArcadeSnakeDemo;
	Runner arcade_paddle = &voxel::RunArcadePaddleDemo;
	Runner arcade_blocks = &voxel::RunArcadeBlocksDemo;
	Runner arcade_invaders = &voxel::RunArcadeInvadersDemo;
	Runner arcade_maze = &voxel::RunArcadeMazeDemo;

	CHECK(desert != nullptr);
	CHECK(aqua != nullptr);
	CHECK(sponza != nullptr);
	CHECK(minecraft != nullptr);
	CHECK(world_edit != nullptr);
	CHECK(showcase != nullptr);
	CHECK(arcade_snake != nullptr);
	CHECK(arcade_paddle != nullptr);
	CHECK(arcade_blocks != nullptr);
	CHECK(arcade_invaders != nullptr);
	CHECK(arcade_maze != nullptr);
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

TEST_CASE("public advanced api configures assets materials entities callbacks")
{
	bool update_called = false;
	bool diagnostics_called = false;
	bool log_called = false;

	voxel::AssetCatalog assets{};
	assets.Texture("grass", "assets/grass.png")
		.Model("crate", "assets/crate.obj")
		.Sound("click", "assets/click.wav");

	voxel::MaterialLibrary materials{};
	materials.Add(voxel::Material::Named("glowing")
		.BaseColor({ -1.0f, 0.8f, 2.0f, 1.0f })
		.Texture("grass")
		.NormalTexture("normal")
		.RoughnessTexture("roughness")
		.MetallicTexture("metallic")
		.OcclusionTexture("occlusion")
		.EmissiveTexture("emissive")
		.Metallic(2.0f)
		.Roughness(-1.0f)
		.Emissive(-2.0f)
		.Transparent());

	voxel::SceneGraph graph{};
	graph.Add(voxel::Entity::Named("crate")
			.At({ 1.0f, 2.0f, 3.0f })
			.Model("crate")
			.Material("glowing"))
		.Add(voxel::Light::Sun({ -1.0f, -2.0f, -1.0f }, 3.0f))
		.EnvironmentSettings(voxel::Environment{}
			.TimeOfDay(18.0f)
			.Fog({ 0.4f, 0.5f, 0.7f, 1.0f }, 0.02f));

	const voxel::EngineConfig config = voxel::EngineConfig::Default()
		.WithAssets(assets)
		.WithMaterials(materials)
		.WithSceneGraph(graph)
		.OnUpdate([&update_called](voxel::FrameContext& frame) {
			update_called = true;
			CHECK(frame.input.IsDown(voxel::Key::Unknown) == false);
			frame.commands.SetBlock(1, 64, 1, voxel::SeaLantern)
				.ClearBox(2, 64, 2, 3, 65, 3)
				.RequestClose();
		})
		.OnDiagnostics([&diagnostics_called](const voxel::Diagnostics& diagnostics) {
			diagnostics_called = true;
			CHECK(diagnostics.render_distance_chunks >= 0);
		})
		.OnLog([&log_called](const std::string& line) {
			log_called = true;
			CHECK(!line.empty());
		});

	CHECK(config.assets.textures.size() == 1);
	CHECK(config.assets.models.size() == 1);
	CHECK(config.assets.sounds.size() == 1);
	REQUIRE(config.materials.materials.size() == 1);
	CHECK(config.materials.materials[0].transparent);
	CHECK(config.materials.materials[0].base_color.r == doctest::Approx(0.0f));
	CHECK(config.materials.materials[0].base_color.b == doctest::Approx(1.0f));
	CHECK(config.materials.materials[0].metallic == doctest::Approx(1.0f));
	CHECK(config.materials.materials[0].roughness == doctest::Approx(0.0f));
	CHECK(config.materials.materials[0].emission == doctest::Approx(0.0f));
	CHECK(config.materials.materials[0].normal_texture == "normal");
	REQUIRE(config.scene_graph.entities.size() == 1);
	CHECK(config.scene_graph.entities[0].transform.position.z == doctest::Approx(3.0f));
	REQUIRE(config.scene_graph.lights.size() == 1);
	CHECK(config.scene_graph.environment.time_of_day == doctest::Approx(18.0f));

	voxel::FrameContext frame{};
	frame.input.f1 = true;
	config.on_update(frame);
	CHECK(update_called);
	CHECK(frame.input.IsDown(voxel::Key::F1));
	CHECK(frame.commands.world_edits.size() == 2);
	CHECK(frame.commands.request_close);

	config.on_diagnostics(voxel::Diagnostics{ 60.0, 2, 4 });
	CHECK(diagnostics_called);

	config.on_log("public api log");
	CHECK(log_called);

	const std::vector<std::string> validation_issues = config.Validate();
	CHECK(std::find(validation_issues.begin(), validation_issues.end(),
		"AssetCatalog runtime loading is not implemented") != validation_issues.end());
	CHECK(std::find(validation_issues.begin(), validation_issues.end(),
		"MaterialLibrary runtime binding is not implemented") != validation_issues.end());
	CHECK(std::find(validation_issues.begin(), validation_issues.end(),
		"SceneGraph runtime rendering is not implemented") != validation_issues.end());
}

TEST_CASE("public config validation reports invalid startup values")
{
	const voxel::EngineConfig invalid = voxel::EngineConfig::Default()
		.WithWindow("Invalid", 0, -1)
		.WithWorldSizeChunks(0)
		.WithRenderDistanceChunks(-1)
		.UseDirectX12()
		.WithAssets(voxel::AssetCatalog{}
			.Texture("duplicate", "a.png")
			.Texture("duplicate", "b.png")
			.Model("", ""));

	const std::vector<std::string> issues = invalid.Validate();

	CHECK(!invalid.IsValid());
	CHECK(issues.size() == 9);
	CHECK(std::find(issues.begin(), issues.end(),
		"AssetCatalog runtime loading is not implemented") != issues.end());
}

TEST_CASE("public engine rejects invalid config before runtime startup")
{
	std::vector<std::string> logs;
	const voxel::EngineConfig invalid = voxel::EngineConfig::Default()
		.WithWindow("Invalid", 0, 720)
		.OnLog([&logs](const std::string& line) {
			logs.push_back(line);
		});

	voxel::Engine engine{ invalid };

	CHECK(engine.Run() == -1);
	REQUIRE(!logs.empty());
	CHECK(logs.front() == "Invalid EngineConfig: window.width must be greater than zero");
}

TEST_CASE("public world serialization roundtrips config edits")
{
	const std::filesystem::path path = std::filesystem::temp_directory_path() / "voxel_public_world_roundtrip.voxelscene";
	const voxel::WorldConfig original = voxel::World(5)
		.SetBlock(1, 2, 3, voxel::DiamondOre)
		.FillBox(-1, 0, -1, 1, 0, 1, voxel::Grass);

	REQUIRE(voxel::SaveWorldConfig(original, path.string()));
	const voxel::WorldConfig loaded = voxel::LoadWorldConfig(path.string());
	std::filesystem::remove(path);

	CHECK(loaded.size_chunks == 5);
	REQUIRE(loaded.edits.size() == 2);
	CHECK(loaded.edits[0].kind == voxel::WorldEdit::Kind::SetBlock);
	CHECK(loaded.edits[0].position.x == 1);
	CHECK(loaded.edits[0].block == voxel::DiamondOre);
	CHECK(loaded.edits[1].kind == voxel::WorldEdit::Kind::FillBox);
	CHECK(loaded.edits[1].box.minimum.x == -1);
	CHECK(loaded.edits[1].block == voxel::Grass);
}
