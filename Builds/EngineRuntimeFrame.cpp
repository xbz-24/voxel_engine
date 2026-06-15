#include "EngineRuntime.h"

#include "OpenGLRenderView.h"

namespace ve::engine
{
	/** Runs gameplay, world rendering, HUD, editor UI, and window presentation once. */
	void EngineRuntime::RunFrame()
	{
		BeginRuntimeFrame();
		OpenGLRenderView& legacy_view = LegacyOpenGLView();
		UpdateGameplay(legacy_view);
		RenderWorld(legacy_view);
		RenderHud(legacy_view);
		EndRuntimeFrame();
	}

	/** Updates frame-scoped state before gameplay systems run. */
	void EngineRuntime::BeginRuntimeFrame()
	{
		engine_.UpdateProjectionIfWindowChanged(window_);
		frame_timer_.Tick();
		editor_controller_.BeginFrame();
	}

	/** Updates gameplay systems that still need the OpenGL block registry. */
	void EngineRuntime::UpdateGameplay(OpenGLRenderView& legacy_view)
	{
		controller_.Update(engine_, window_, *model_, legacy_view.MutableBlockRegistry(), frame_timer_.DeltaSeconds());
	}

	/** Renders the voxel world through the OpenGL compatibility path. */
	void EngineRuntime::RenderWorld(OpenGLRenderView& legacy_view)
	{
		engine_.Render3DWorld(window_, model_->MutableCamera(), legacy_view.MutableSkyBox(), legacy_view.MutablePlane(),
			legacy_view.MutableSelectionCube(), legacy_view.MutableBlockRegistry(), model_->MutableWorld(), model_->GetSelection());
	}

	/** Renders the HUD through the OpenGL compatibility path. */
	void EngineRuntime::RenderHud(OpenGLRenderView& legacy_view)
	{
		legacy_view.MutableHudRenderer().Draw(engine_.CreateHudFrame(window_, model_->GetCamera(), frame_timer_,
			model_->GetSelection(), legacy_view.MutableBlockRegistry(), model_->GetWorld()));
	}

	/** Renders editor panels and presents the native window. */
	void EngineRuntime::EndRuntimeFrame()
	{
		editor_controller_.DrawAndApply(window_, engine_._runtimeSettings);
		editor_controller_.Render();
		window_.Update();
	}
}
