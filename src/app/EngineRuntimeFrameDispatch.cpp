#include "EngineRuntime.h"

#include "RuntimeRenderFrame.h"

namespace ve::engine
{
	/** Runs gameplay, world rendering, HUD, editor UI, and window presentation once. */
	void EngineRuntime::RunFrame()
	{
		BeginRuntimeFrame();
		RuntimeRenderFrame frame{ window_, *model_, controller_, editor_controller_,
			runtime_input_actions_, engine_.MutableRuntimeSettings(), frame_timer_ };
		render_driver_->BeginFrame(frame, *this);
		render_driver_->UpdateGameplay(frame);
		ApplyConfiguredWorldEditsOnce();
		InvokePublicApiFrameCallbacks();
		if (!render_driver_->DrawFrame(frame, *this)) window_.Close();
		render_driver_->EndFrame(frame);
		EndRuntimeFrame();
	}
}
