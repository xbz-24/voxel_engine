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
