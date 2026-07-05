#include <doctest/doctest.h>

#include "GraphicsFacade.h"
#include "GraphicsResourcePool.h"
#include "MortonCode.h"
#include "RenderCommandSorter.h"

#include <cstddef>
#include <string>
#include <type_traits>
#include <variant>

namespace
{
	class RecordingRenderCommandExecutor final : public ve::rendering::RenderCommandExecutor
	{
	public:
		/** @param command_list Submitted commands inspected by this test double. */
		void Execute(const ve::rendering::RenderCommandList& command_list) override
		{
			executed_command_count += command_list.Count();

			for (const ve::rendering::RenderCommand& command : command_list.Commands())
			{
				std::visit(
					[this](const auto& payload) {
						using PayloadType = std::decay_t<decltype(payload)>;
						if constexpr (std::is_same_v<PayloadType, ve::rendering::DrawTriangle2DCommand>)
						{
							++triangle_command_count;
						}
						else if constexpr (std::is_same_v<PayloadType, ve::rendering::DrawRect2DCommand>)
						{
							++rect_command_count;
						}
						else if constexpr (std::is_same_v<PayloadType, ve::rendering::DrawTexturedQuad2DCommand>)
						{
							++textured_quad_command_count;
						}
						else if constexpr (std::is_same_v<PayloadType, ve::rendering::DrawText2DCommand>)
						{
							++text_command_count;
						}
						else if constexpr (std::is_same_v<PayloadType, ve::rendering::DrawCube3DCommand>)
						{
							++cube_command_count;
						}
						else if constexpr (std::is_same_v<PayloadType, ve::rendering::ScissorRectCommand>)
						{
							++scissor_command_count;
						}
					},
					command.payload);
			}
		}

		std::size_t executed_command_count = 0;
		std::size_t triangle_command_count = 0;
		std::size_t rect_command_count = 0;
		std::size_t textured_quad_command_count = 0;
		std::size_t text_command_count = 0;
		std::size_t cube_command_count = 0;
		std::size_t scissor_command_count = 0;
	};
}

TEST_CASE("resource pool rejects stale handles")
{
	ve::rendering::GraphicsResourcePool<int> resources;
	const ve::rendering::GraphicsResourceHandle first_handle = resources.Create(42);

	REQUIRE(resources.TryGet(first_handle) != nullptr);
	CHECK(*resources.TryGet(first_handle) == 42);
	CHECK(resources.Destroy(first_handle));
	CHECK(resources.TryGet(first_handle) == nullptr);
	CHECK(resources.Count() == 0U);
}

TEST_CASE("morton code roundtrips 3d coordinates")
{
	const ve::rendering::MortonCoordinate coordinate{ 17U, 9U, 5U };
	const std::uint64_t encoded = ve::rendering::EncodeMorton3D(coordinate);
	const ve::rendering::MortonCoordinate decoded = ve::rendering::DecodeMorton3D(encoded);

	CHECK(decoded.x == coordinate.x);
	CHECK(decoded.y == coordinate.y);
	CHECK(decoded.z == coordinate.z);
}

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
	CHECK(text_command->text == "HUD");
}

TEST_CASE("graphics facade records and submits common primitives")
{
	RecordingRenderCommandExecutor executor;
	ve::rendering::GraphicsFacade graphics{ executor };

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
