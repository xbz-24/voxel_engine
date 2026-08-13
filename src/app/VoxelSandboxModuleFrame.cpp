#include "VoxelSandboxModule.h"

#include "GameModel.h"
#include "FrameTimer.h"
#include "RuntimeRenderDriver.h"
#include "RuntimeRenderFrame.h"
#include "Window.h"

namespace ve::engine
{
	void VoxelSandboxModule::RunFrame(RuntimeModuleContext& context)
	{
		window_ = &context.window;
		frame_timer_ = &context.frame_timer;
		elapsed_seconds_ += context.frame_timer.DeltaSeconds();
		RuntimeRenderFrame frame{ context.window, *model_, controller_, editor_controller_,
			runtime_input_actions_, runtime_settings_, context.frame_timer };
		render_driver_->BeginFrame(frame, *this);
		render_driver_->UpdateGameplay(frame);
		ApplyConfiguredWorldEditsOnce();
		InvokePublicApiFrameCallbacks(context);
		if (!render_driver_->DrawFrame(frame, *this)) context.window.Close();
		render_driver_->EndFrame(frame);
	}
}
