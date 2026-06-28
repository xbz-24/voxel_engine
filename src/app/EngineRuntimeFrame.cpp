#include "EngineRuntime.h"

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
		const bool ui_captures_mouse = vulkan_frame_renderer_.WantsMouseInput();
		const bool ui_captures_keyboard = vulkan_frame_renderer_.WantsKeyboardInput();
		engine_._runtimeSettings.isSettingsMenuOpen = ui_captures_mouse;

		const ve::blocks::BlockRegistry* block_registry = model_->GetBlockRegistry();
		assert(block_registry != nullptr);
		controller_.UpdateVulkanDemo(window_, *model_, *block_registry, engine_._runtimeSettings, vulkan_demo_settings_,
			frame_timer_.DeltaSeconds(), ui_captures_keyboard);
		ApplyConfiguredWorldEditsOnce();
		InvokePublicApiFrameCallbacks();

		if (!DrawVulkanFrame(CaptureVulkanDemoInput()))
		{
			window_.Close();
		}

		window_.SetCursorMode(vulkan_demo_settings_.show_controls ? Window::CursorMode::Normal : Window::CursorMode::Captured);
	}

	ve::rendering::VulkanDemoInput EngineRuntime::CaptureVulkanDemoInput()
	{
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
		return ve::rendering::VulkanDemoInput{
			mouse_x * scale_x,
			mouse_y * scale_y,
			vulkan_input_.IsDown(VulkanInputController::Action::LeftClick),
			vulkan_input_.IsJustPressed(VulkanInputController::Action::LeftClick),
			vulkan_input_.IsJustPressed(VulkanInputController::Action::F1),
			vulkan_input_.IsJustPressed(VulkanInputController::Action::F2)
		};
	}

	bool EngineRuntime::DrawVulkanFrame(const ve::rendering::VulkanDemoInput& input)
	{
		return vulkan_frame_renderer_.DrawFrame(model_->GetWorld(),
			model_->GetCamera(),
			frame_timer_.DisplayedFps(),
			frame_timer_.DeltaSeconds(),
			input,
			vulkan_demo_settings_);
	}

	/** Updates frame-scoped state before gameplay systems run. */
	void EngineRuntime::BeginRuntimeFrame()
	{
		frame_timer_.Tick();
		elapsed_seconds_ += frame_timer_.DeltaSeconds();
		if (view_->Api() == ve::rendering::GraphicsApi::OpenGLCompatibility)
		{
			engine_.UpdateProjectionIfWindowChanged(window_);
			editor_controller_.BeginFrame();
		}
	}

	void EngineRuntime::InvokePublicApiFrameCallbacks()
	{
		if (!engine_.create_info_.on_update && !engine_.create_info_.on_diagnostics) return;

		double mouse_x = 0.0;
		double mouse_y = 0.0;
		GLFWwindow* native_window = window_.GetNativeWindow();
		glfwGetCursorPos(native_window, &mouse_x, &mouse_y);

		RuntimeInputSnapshot input{};
		input.move_forward = glfwGetKey(native_window, GLFW_KEY_W) == GLFW_PRESS;
		input.move_left = glfwGetKey(native_window, GLFW_KEY_A) == GLFW_PRESS;
		input.move_back = glfwGetKey(native_window, GLFW_KEY_S) == GLFW_PRESS;
		input.move_right = glfwGetKey(native_window, GLFW_KEY_D) == GLFW_PRESS;
		input.jump = glfwGetKey(native_window, GLFW_KEY_SPACE) == GLFW_PRESS;
		input.escape = glfwGetKey(native_window, GLFW_KEY_ESCAPE) == GLFW_PRESS;
		input.f1 = glfwGetKey(native_window, GLFW_KEY_F1) == GLFW_PRESS;
		input.f2 = glfwGetKey(native_window, GLFW_KEY_F2) == GLFW_PRESS;
		input.primary_action = glfwGetMouseButton(native_window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS;
		input.mouse_x = mouse_x;
		input.mouse_y = mouse_y;

		if (engine_.create_info_.on_update)
		{
			RuntimeFrameContext frame{};
			frame.delta_seconds = static_cast<float>(frame_timer_.DeltaSeconds());
			frame.elapsed_seconds = static_cast<float>(elapsed_seconds_);
			frame.fps = frame_timer_.DisplayedFps();
			frame.input = input;
			engine_.create_info_.on_update(frame);
			ApplyWorldEdits(frame.world_edits);
			if (frame.request_close) window_.Close();
		}

		if (engine_.create_info_.on_diagnostics)
		{
			const size_t pending_event_count = model_ != nullptr ? model_->GetWorld().PendingEventCount() : 0U;
			const int pending_events = pending_event_count > static_cast<size_t>(std::numeric_limits<int>::max())
				? std::numeric_limits<int>::max()
				: static_cast<int>(pending_event_count);
			engine_.create_info_.on_diagnostics(RuntimeDiagnostics{
				static_cast<double>(frame_timer_.DisplayedFps()),
				pending_events,
				engine_._runtimeSettings.renderDistanceChunks
			});
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
	void EngineRuntime::RenderWorld(RenderView& render_view)
	{
		(void)render_view;
		// TODO: Move world rendering behind RenderView so this path stops downcasting back to OpenGLRenderView.
		ve::blocks::BlockRegistry* block_registry = model_->MutableBlockRegistry();
		assert(block_registry != nullptr);
		OpenGLRenderView& legacy_view = LegacyOpenGLView();
		SkyBox* skybox = legacy_view.Skybox();
		Plane* ground_plane = legacy_view.GroundPlane();
		BlockSelectionCube* selection_cube = legacy_view.SelectionCube();
		assert(skybox != nullptr);
		assert(ground_plane != nullptr);
		assert(selection_cube != nullptr);
		engine_.Render3DWorld(window_, model_->MutableCamera(), *skybox, *ground_plane,
			*selection_cube, *block_registry, model_->MutableWorld(), model_->GetSelection());
	}

	/** Renders the HUD through the OpenGL compatibility path. */
	void EngineRuntime::RenderHud(RenderView& render_view)
	{
		(void)render_view;
		// TODO: Introduce a backend-neutral HUD renderer so Vulkan and OpenGL share the same HudFrameInfo pipeline.
		const ve::blocks::BlockRegistry* block_registry = model_->GetBlockRegistry();
		assert(block_registry != nullptr);
		ve::ui::HudRenderer* hud = LegacyOpenGLView().Hud();
		assert(hud != nullptr);
		hud->Draw(engine_.CreateHudFrame(window_, model_->GetCamera(), frame_timer_,
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
