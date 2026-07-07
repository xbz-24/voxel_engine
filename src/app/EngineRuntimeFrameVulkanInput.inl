	ve::rendering::VulkanDemoInput EngineRuntime::CaptureVulkanDemoInput()
	{
		int window_width = 1;
		int window_height = 1;
		const RuntimeInputSnapshot runtime_input_snapshot = CaptureRuntimeInputSnapshot(window_);

		const Window::WindowSize client_window_size = window_.ClientWindowSize();
		window_width = client_window_size.width;
		window_height = client_window_size.height;

		const double scale_x = static_cast<double>(window_.GetWidth()) / static_cast<double>(std::max(window_width, 1));
		const double scale_y = static_cast<double>(window_.GetHeight()) / static_cast<double>(std::max(window_height, 1));

		runtime_input_actions_.Update(runtime_input_snapshot);
		return ve::rendering::VulkanDemoInput{
			runtime_input_snapshot.mouse_x * scale_x,
			runtime_input_snapshot.mouse_y * scale_y,
			runtime_input_actions_.IsDown(RuntimeInputAction::PrimaryAction),
			runtime_input_actions_.WasJustPressed(RuntimeInputAction::PrimaryAction),
			runtime_input_actions_.WasJustPressed(RuntimeInputAction::ToggleDebugOverlay),
			runtime_input_actions_.WasJustPressed(RuntimeInputAction::ToggleRenderMode)
		};
	}

	bool EngineRuntime::DrawVulkanFrame(const ve::rendering::VulkanDemoInput& input)
	{
		const ve::blocks::BlockRegistry* block_registry = model_->GetBlockRegistry();
		assert(block_registry != nullptr);
		return vulkan_frame_orchestrator_.DrawFrame(model_->GetWorld(),
			*block_registry,
			model_->GetCamera(),
			frame_timer_.DisplayedFps(),
			frame_timer_.DeltaSeconds(),
			input,
			vulkan_demo_settings_);
	}
