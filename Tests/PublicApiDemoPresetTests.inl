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
