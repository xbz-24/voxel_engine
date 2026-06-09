#include "EngineRuntime.h"

namespace ve::engine
{
	/** Runs gameplay, world rendering, HUD, editor UI, and window presentation once. */
	void EngineRuntime::RunFrame()
	{
		engine_.UpdateProjectionIfWindowChanged(window_);
		frame_timer_.Tick();
		editor_controller_.BeginFrame();
		controller_.Update(engine_, window_, *model_, view_->MutableBlockRegistry(), frame_timer_.DeltaSeconds());
		engine_.Render3DWorld(window_, model_->MutableCamera(), view_->MutableSkyBox(), view_->MutablePlane(),
			view_->MutableSelectionCube(), view_->MutableBlockRegistry(), model_->MutableWorld(), model_->GetSelection());
		view_->MutableHudRenderer().Draw(engine_.CreateHudFrame(window_, model_->GetCamera(), frame_timer_,
			model_->GetSelection(), view_->MutableBlockRegistry(), model_->GetWorld()));
		editor_controller_.DrawAndApply(window_, engine_._runtimeSettings);
		editor_controller_.Render();
		window_.Update();
	}
}
