#include "EngineRuntime.h"

#include "OpenGLRenderView.h"

#include <cassert>

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
		UpdateGameplay();
		RenderWorld(legacy_view);
		RenderHud(legacy_view);
	}

	/** Presents the Vulkan migration frame. */
	void EngineRuntime::RunVulkanFrame()
	{
		controller_.UpdateVulkanDemo(window_, *model_, frame_timer_.DeltaSeconds());
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
	void EngineRuntime::UpdateGameplay()
	{
		ve::blocks::BlockRegistry* block_registry = model_->MutableBlockRegistry();
		assert(block_registry != nullptr);
		controller_.Update(engine_, window_, *model_, *block_registry, frame_timer_.DeltaSeconds());
	}

	/** Renders the voxel world through the OpenGL compatibility path. */
	void EngineRuntime::RenderWorld(OpenGLRenderView& legacy_view)
	{
		ve::blocks::BlockRegistry* block_registry = model_->MutableBlockRegistry();
		assert(block_registry != nullptr);
		engine_.Render3DWorld(window_, model_->MutableCamera(), legacy_view.MutableSkyBox(), legacy_view.MutablePlane(),
			legacy_view.MutableSelectionCube(), *block_registry, model_->MutableWorld(), model_->GetSelection());
	}

	/** Renders the HUD through the OpenGL compatibility path. */
	void EngineRuntime::RenderHud(OpenGLRenderView& legacy_view)
	{
		const ve::blocks::BlockRegistry* block_registry = model_->GetBlockRegistry();
		assert(block_registry != nullptr);
		legacy_view.MutableHudRenderer().Draw(engine_.CreateHudFrame(window_, model_->GetCamera(), frame_timer_,
			model_->GetSelection(), *block_registry, model_->GetWorld()));
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
