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
