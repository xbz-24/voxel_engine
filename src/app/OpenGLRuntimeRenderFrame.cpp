#include "OpenGLRuntimeRenderDriver.h"

#include "EditorRuntimeController.h"
#include "FrameTimer.h"
#include "GameController.h"
#include "GameModel.h"
#include "RuntimeRenderFrame.h"
#include "RuntimeRenderHost.h"

#include <cassert>

namespace ve::engine
{
	void OpenGLRuntimeRenderDriver::BeginFrame(RuntimeRenderFrame& frame, RuntimeRenderHost& host)
	{
		host.UpdateViewportProjection();
		frame.editor.BeginFrame();
	}

	void OpenGLRuntimeRenderDriver::UpdateGameplay(RuntimeRenderFrame& frame)
	{
		const ve::blocks::BlockRegistry* registry = frame.model.GetBlockRegistry();
		assert(registry != nullptr);
		frame.controller.Update(frame.window, frame.model, *registry,
			frame.settings, frame.timer.DeltaSeconds());
	}

	bool OpenGLRuntimeRenderDriver::DrawFrame(RuntimeRenderFrame&, RuntimeRenderHost& host)
	{
		assert(view_ != nullptr);
		host.RenderWorld(*view_);
		host.RenderHud(*view_);
		return true;
	}

	void OpenGLRuntimeRenderDriver::EndFrame(RuntimeRenderFrame& frame)
	{
		frame.editor.DrawAndApply(frame.window, frame.settings);
		frame.editor.Render();
	}
}
