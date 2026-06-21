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
		const bool ui_captures_input = vulkan_demo_settings_.show_controls || vulkan_frame_renderer_.WantsMouseInput();
		engine_._runtimeSettings.isSettingsMenuOpen = ui_captures_input;

		const ve::blocks::BlockRegistry* block_registry = model_->GetBlockRegistry();
		assert(block_registry != nullptr);
		controller_.UpdateVulkanDemo(window_, *model_, *block_registry, engine_._runtimeSettings, vulkan_demo_settings_,
			frame_timer_.DeltaSeconds(), ui_captures_input);

		double mouse_x = 0.0;
		double mouse_y = 0.0;
		int window_width = 1;
		int window_height = 1;
		GLFWwindow* native_window = window_.GetNativeWindow();

		glfwGetCursorPos(native_window, &mouse_x, &mouse_y);
		glfwGetWindowSize(native_window, &window_width, &window_height);

		const double scale_x = static_cast<double>(window_.GetWidth()) / static_cast<double>(std::max(window_width, 1));
		const double scale_y = static_cast<double>(window_.GetHeight()) / static_cast<double>(std::max(window_height, 1));

		vulkan_input_.Update(native_window);

		const ve::rendering::VulkanDemoInput input{
			mouse_x * scale_x,
			mouse_y * scale_y,
			vulkan_input_.IsDown(VulkanInputController::Action::LeftClick),
			vulkan_input_.IsJustPressed(VulkanInputController::Action::LeftClick),
			vulkan_input_.IsJustPressed(VulkanInputController::Action::F1),
			vulkan_input_.IsJustPressed(VulkanInputController::Action::F2)
		};

		if (!vulkan_frame_renderer_.DrawFrame(model_->GetWorld(),
			model_->GetCamera(),
			frame_timer_.DisplayedFps(),
			frame_timer_.DeltaSeconds(),
			input,
			vulkan_demo_settings_))
		{
			window_.Close();
		}

		window_.SetCursorMode(vulkan_demo_settings_.show_controls ? Window::CursorMode::Normal : Window::CursorMode::Captured);
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
		controller_.Update(window_, *model_, *block_registry, engine_._runtimeSettings, frame_timer_.DeltaSeconds());
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
			model_->GetSelection(), *block_registry, model_->GetWorld(), controller_.SelectedPlacementBlock()));
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
