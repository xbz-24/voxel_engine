#include "EngineRuntime.h"

#include "Input.h"
#include "OpenGLRenderView.h"
#include "PlayerMovementInput.h"

#include <algorithm>

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
		OpenGLRenderView& legacy_view = LegacyOpenGLView();
		UpdateGameplay(legacy_view);
		RenderWorld(legacy_view);
		RenderHud(legacy_view);
	}

	/** Presents the Vulkan migration frame. */
	void EngineRuntime::RunVulkanFrame()
	{
		if (ve::input::IsPressed(window_.GetNativeWindow(), ve::input::Key::Escape)) window_.Close();
		model_->PumpAsyncWorldGeneration();
		const float speed = 18.0f * static_cast<float>(std::max(frame_timer_.DeltaSeconds(), 0.001));
		const ve::gameplay::PlayerMoveIntent intent = ve::gameplay::ReadPlayerMoveIntent(window_.GetNativeWindow());
		ve::gameplay::ApplyPlanarMovement(intent, model_->MutableCamera(), speed);
		ve::gameplay::ApplyFlyingMovement(intent, model_->MutableCamera(), speed);
		if (!vulkan_frame_renderer_.DrawFrame(model_->GetWorld(), model_->GetCamera())) window_.Close();
	}

	/** Updates frame-scoped state before gameplay systems run. */
	void EngineRuntime::BeginRuntimeFrame()
	{
		frame_timer_.Tick();
		if (view_->Api() == ve::rendering::GraphicsApi::OpenGLCompatibility)
		{
			engine_.UpdateProjectionIfWindowChanged(window_);
			editor_controller_.BeginFrame();
		}
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
		if (view_->Api() == ve::rendering::GraphicsApi::OpenGLCompatibility)
		{
			editor_controller_.DrawAndApply(window_, engine_._runtimeSettings);
			editor_controller_.Render();
		}
		window_.Update();
	}
}
