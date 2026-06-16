#include "EngineRuntime.h"

#include "OpenGLRenderView.h"

#include <algorithm>
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
		static bool was_left_down = false;
		static bool was_f1_down = false;
		static bool was_f2_down = false;
		controller_.UpdateVulkanDemo(window_, *model_, frame_timer_.DeltaSeconds());
		double mouse_x = 0.0;
		double mouse_y = 0.0;
		int window_width = 1;
		int window_height = 1;
		GLFWwindow* native_window = window_.GetNativeWindow();
		glfwGetCursorPos(native_window, &mouse_x, &mouse_y);
		glfwGetWindowSize(native_window, &window_width, &window_height);
		const double scale_x = static_cast<double>(window_.GetWidth()) / static_cast<double>(std::max(window_width, 1));
		const double scale_y = static_cast<double>(window_.GetHeight()) / static_cast<double>(std::max(window_height, 1));
		const bool left_down = glfwGetMouseButton(native_window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS;
		const bool f1_down = glfwGetKey(native_window, GLFW_KEY_F1) == GLFW_PRESS;
		const bool f2_down = glfwGetKey(native_window, GLFW_KEY_F2) == GLFW_PRESS;
		const ve::rendering::VulkanDemoInput input{
			mouse_x * scale_x,
			mouse_y * scale_y,
			left_down,
			left_down && !was_left_down,
			f1_down && !was_f1_down,
			f2_down && !was_f2_down
		};
		was_left_down = left_down;
		was_f1_down = f1_down;
		was_f2_down = f2_down;
		if (!vulkan_frame_renderer_.DrawFrame(model_->GetWorld(), model_->GetCamera(), frame_timer_.DisplayedFps(), frame_timer_.DeltaSeconds(), input)) window_.Close();
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
