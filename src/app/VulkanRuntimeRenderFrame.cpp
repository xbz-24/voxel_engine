#include "VulkanRuntimeRenderDriver.h"

#include "FrameTimer.h"
#include "GameController.h"
#include "GameModel.h"
#include "RuntimeRenderFrame.h"
#include "RuntimeRenderHost.h"
#include "Window.h"

#include <cassert>

namespace ve::engine
{
	void VulkanRuntimeRenderDriver::BeginFrame(RuntimeRenderFrame&, RuntimeRenderHost&) {}

	void VulkanRuntimeRenderDriver::UpdateGameplay(RuntimeRenderFrame& frame)
	{
		const bool captures_mouse = orchestrator_.WantsMouseInput();
		const bool captures_keyboard = orchestrator_.WantsKeyboardInput();
		frame.settings.editor.is_settings_menu_open = captures_mouse;
		const ve::blocks::BlockRegistry* registry = frame.model.GetBlockRegistry();
		assert(registry != nullptr);
		frame.controller.UpdateVulkanWorld(frame.window, frame.model, *registry,
			frame.settings, frame.timer.DeltaSeconds(), captures_keyboard);
	}

	bool VulkanRuntimeRenderDriver::DrawFrame(RuntimeRenderFrame& frame, RuntimeRenderHost&)
	{
		const ve::blocks::BlockRegistry* registry = frame.model.GetBlockRegistry();
		assert(registry != nullptr);
		return orchestrator_.DrawFrame(frame.model.GetWorld(), *registry,
			frame.model.GetCamera(), frame.timer.DisplayedFps(), frame.timer.DeltaSeconds(),
			CaptureInput(frame), overlay_settings_);
	}

	void VulkanRuntimeRenderDriver::EndFrame(RuntimeRenderFrame& frame)
	{
		frame.window.SetCursorMode(orchestrator_.WantsMouseInput()
			? Window::CursorMode::Normal
			: Window::CursorMode::Captured);
	}
}
