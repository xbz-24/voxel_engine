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
