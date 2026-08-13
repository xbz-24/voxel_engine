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

