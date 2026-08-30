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

