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
