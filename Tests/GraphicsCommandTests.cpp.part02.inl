
TEST_CASE("render command list sorts by submission key")
{
	ve::rendering::RenderCommandList commands;
	commands.DrawSolidRect({ 0.0f, 0.0f, 1.0f, 1.0f }, {}, { .layer = 2, .material = 2 });
	commands.DrawSolidRect({ 0.0f, 0.0f, 1.0f, 1.0f }, {}, { .layer = 1, .material = 9 });
	commands.DrawTriangle(
		{ { 0.0f, 0.0f }, { 1.0f, 0.0f }, { 0.0f, 1.0f } }, {},
		{ .layer = 2, .material = 1 });

	ve::rendering::RenderCommandSorter::SortForSubmission(commands.MutableCommands());

	REQUIRE(commands.Count() == 3U);
	CHECK(commands.Commands()[0].sort_key.layer == 1);
	CHECK(commands.Commands()[1].sort_key.material == 1U);
	CHECK(commands.Commands()[2].sort_key.material == 2U);
}

TEST_CASE("render command list reuses frame arena for text commands")
{
	ve::rendering::RenderCommandList commands;
	commands.Reserve(2U);
	commands.DrawText(std::string(2048U, 'x'), { 0.0f, 0.0f }, 1.0f, {});
	commands.Clear();

	commands.DrawText("HUD", { 8.0f, 12.0f }, 1.0f, {});

	REQUIRE(commands.Count() == 1U);
	const auto* text_command = std::get_if<ve::rendering::DrawText2DCommand>(&commands.Commands()[0].payload);
	REQUIRE(text_command != nullptr);
	CHECK(text_command->text.compare("HUD") == 0);
}

TEST_CASE("graphics facade records and submits common primitives")
{
	RecordingRenderCommandExecutor executor;
	ve::rendering::GraphicsFacade graphics{ executor };

	CHECK(ve::rendering::GraphicsFacade::Role() == ve::rendering::GraphicsFacadeRole::ImmediateCommandDrawing);
	graphics.BeginFrame();
	graphics.DrawTriangle({ { 0.0f, 0.0f }, { 1.0f, 0.0f }, { 0.0f, 1.0f } }, {});
	graphics.DrawRectangle({ 0.0f, 0.0f, 10.0f, 10.0f }, {});
	graphics.DrawCube({ 0.0f, 0.0f, 0.0f }, { 1.0f, 1.0f, 1.0f }, {});
	graphics.Submit();

	CHECK(graphics.PendingCommandCount() == 3U);
	CHECK(executor.executed_command_count == 3U);
	CHECK(executor.triangle_command_count == 1U);
	CHECK(executor.rect_command_count == 1U);
	CHECK(executor.cube_command_count == 1U);
}

TEST_CASE("graphics facade keeps separate command lists per layer")
{
	RecordingRenderCommandExecutor executor;
	ve::rendering::GraphicsFacade graphics{ executor };

	graphics.BeginFrame();
	graphics.DrawRectangle({ 0.0f, 0.0f, 10.0f, 10.0f }, {});
	graphics.SetScissorRect({ 0.0f, 0.0f, 320.0f, 200.0f });
	graphics.DrawTexturedQuad(
		ve::rendering::TextureHandle{ 42U },
		{ 4.0f, 6.0f, 32.0f, 32.0f },
		{});
	graphics.DrawText("HUD", { 8.0f, 12.0f }, 1.0f, {});

	CHECK(graphics.Commands(ve::rendering::GraphicsCommandLayer::World).Count() == 1U);
	CHECK(graphics.Commands(ve::rendering::GraphicsCommandLayer::Hud).Count() == 3U);
	CHECK(graphics.PendingCommandCount() == 4U);

	graphics.Submit();

	CHECK(executor.executed_command_count == 4U);
	CHECK(executor.rect_command_count == 1U);
	CHECK(executor.scissor_command_count == 1U);
	CHECK(executor.textured_quad_command_count == 1U);
	CHECK(executor.text_command_count == 1U);
}
