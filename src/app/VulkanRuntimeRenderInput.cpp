#include "VulkanRuntimeRenderDriver.h"

#include "CoreTypes.h"
#include "RuntimeInput.h"
#include "RuntimeRenderFrame.h"
#include "Window.h"

#include <algorithm>

namespace ve::engine
{
	ve::rendering::VulkanFrameInput VulkanRuntimeRenderDriver::CaptureInput(RuntimeRenderFrame& frame)
	{
		const RuntimeInputSnapshot snapshot = CaptureRuntimeInputSnapshot(frame.window);
		const Window::WindowSize client_size = frame.window.ClientWindowSize();
		const double scale_x = ve::core::ToDouble(frame.window.GetWidth()) /
			ve::core::ToDouble(std::max(client_size.width, 1));
		const double scale_y = ve::core::ToDouble(frame.window.GetHeight()) /
			ve::core::ToDouble(std::max(client_size.height, 1));
		frame.input_actions.Update(snapshot);
		return ve::rendering::VulkanFrameInput{
			snapshot.mouse_x * scale_x,
			snapshot.mouse_y * scale_y,
			frame.input_actions.IsDown(RuntimeInputAction::PrimaryAction),
			frame.input_actions.WasJustPressed(RuntimeInputAction::PrimaryAction),
			frame.input_actions.WasJustPressed(RuntimeInputAction::ToggleDebugOverlay),
			frame.input_actions.WasJustPressed(RuntimeInputAction::ToggleRenderMode)
		};
	}
}
