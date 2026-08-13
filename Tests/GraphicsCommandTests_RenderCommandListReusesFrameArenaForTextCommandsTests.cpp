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

