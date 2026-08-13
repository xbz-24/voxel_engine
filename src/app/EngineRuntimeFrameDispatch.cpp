#include "EngineRuntime.h"

#include "CoreTypes.h"
#include "OpenGLRenderView.h"

#include <algorithm>
#include <cassert>
#include <limits>

namespace ve::engine
{
	/** Runs gameplay, world rendering, HUD, editor UI, and window presentation once. */
	void EngineRuntime::RunFrame()
	{
		BeginRuntimeFrame();
		if (view_->Api() == ve::rendering::GraphicsApi::Vulkan)
		{
			RunVulkanFrame();
		}
		else
		{
			RunOpenGLFrame();
		}
		EndRuntimeFrame();
	}

	/** Runs gameplay, world, and HUD rendering through the OpenGL compatibility view. */
	void EngineRuntime::RunOpenGLFrame()
	{
		RenderView& render_view = *view_;
		UpdateGameplay();
		ApplyConfiguredWorldEditsOnce();
		InvokePublicApiFrameCallbacks();
		RenderWorld(render_view);
		RenderHud(render_view);
	}

	/** Presents the Vulkan migration frame. */
	void EngineRuntime::RunVulkanFrame()
	{
		const bool ui_captures_mouse = vulkan_frame_orchestrator_.WantsMouseInput();
		const bool ui_captures_keyboard = vulkan_frame_orchestrator_.WantsKeyboardInput();
		ve::gameplay::RuntimeSettings& runtime_settings = engine_.MutableRuntimeSettings();
		runtime_settings.editor.is_settings_menu_open = ui_captures_mouse;

		const ve::blocks::BlockRegistry* block_registry = model_->GetBlockRegistry();
		assert(block_registry != nullptr);
		controller_.UpdateVulkanWorld(window_, *model_, *block_registry, runtime_settings,
			frame_timer_.DeltaSeconds(), ui_captures_keyboard);
		ApplyConfiguredWorldEditsOnce();
		InvokePublicApiFrameCallbacks();

		if (!DrawVulkanFrame(CaptureVulkanFrameInput()))
		{
			window_.Close();
		}

		window_.SetCursorMode(vulkan_frame_orchestrator_.WantsMouseInput()
			? Window::CursorMode::Normal
			: Window::CursorMode::Captured);
	}
}
