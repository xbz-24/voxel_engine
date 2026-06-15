#include "OpenGLRenderCommandExecutor.h"

#include "Render2D.h"
#include "RenderPrimitives3D.h"

#include <variant>

namespace ve::rendering
{
	namespace
	{
		/** @param command Filled triangle command to execute. */
		void ExecuteCommand(const DrawTriangle2DCommand& command)
		{
			Canvas2D{}.DrawSolidTriangle(command.triangle, command.color);
		}

		/** @param command Rectangle command to execute. */
		void ExecuteCommand(const DrawRect2DCommand& command)
		{
			if (command.is_filled) Canvas2D{}.DrawSolidRect(command.rect, command.color);
			else Canvas2D{}.DrawRectOutline(command.rect, command.color, command.line_width);
		}

		/** @param command Cube command to execute. */
		void ExecuteCommand(const DrawCube3DCommand& command)
		{
			const CubeRequest request{ command.center, command.size, command.color };
			if (command.is_filled) PrimitiveRenderer3D{}.DrawSolidCube(request);
			else PrimitiveRenderer3D{}.DrawWireCube(request, command.line_width);
		}
	}

	/** Executes each queued command through the compatibility renderer. */
	void OpenGLRenderCommandExecutor::Execute(const RenderCommandList& command_list)
	{
		for (const RenderCommand& command : command_list.Commands())
		{
			std::visit([](const auto& payload) { ExecuteCommand(payload); }, command.payload);
		}
	}
}
