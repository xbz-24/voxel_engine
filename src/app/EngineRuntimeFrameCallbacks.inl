	namespace
	{
		[[nodiscard]] int ToDiagnosticCounter(std::size_t value) noexcept
		{
			return value > static_cast<std::size_t>(std::numeric_limits<int>::max())
				? std::numeric_limits<int>::max()
				: static_cast<int>(value);
		}
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
		const EngineCreateInfo& create_info = engine_.CreateInfo();
		if (!create_info.on_update && !create_info.on_diagnostics) return;

		GLFWwindow* native_window = window_.GetNativeWindow();
		const RuntimeInputSnapshot runtime_input_snapshot = CaptureRuntimeInputSnapshot(native_window);

		if (create_info.on_update)
		{
			RuntimeFrameContext runtime_frame_context{};
			runtime_frame_context.delta_seconds = static_cast<float>(frame_timer_.DeltaSeconds());
			runtime_frame_context.elapsed_seconds = static_cast<float>(elapsed_seconds_);
			runtime_frame_context.fps = frame_timer_.DisplayedFps();
			runtime_frame_context.input = runtime_input_snapshot;
			if (model_ != nullptr)
			{
				const Camera& camera = model_->GetCamera();
				runtime_frame_context.camera.position = camera.GetPosition();
				runtime_frame_context.camera.forward = camera.GetForward();
				runtime_frame_context.selected_block = controller_.SelectedPlacementBlock();

				const ve::gameplay::BlockSelection& selection = model_->GetSelection();
				runtime_frame_context.hit_result.has_hit = selection.has_target;
				runtime_frame_context.hit_result.target_block = selection.target_block;
				runtime_frame_context.hit_result.placement_block = selection.placement_block;
				if (selection.has_target)
				{
					runtime_frame_context.hit_result.target_block_id = model_->GetWorld().GetBlock(selection.target_block);
				}
			}
			create_info.on_update(runtime_frame_context);
			ApplyWorldEdits(runtime_frame_context.world_edits);
			if (runtime_frame_context.request_close) window_.Close();
		}

		if (create_info.on_diagnostics)
		{
			const ve::world::WorldMetrics world_metrics =
				model_ != nullptr ? model_->GetWorldMetrics() : ve::world::WorldMetrics{};
			create_info.on_diagnostics(RuntimeDiagnostics{
				static_cast<double>(frame_timer_.DisplayedFps()),
				ToDiagnosticCounter(world_metrics.pendingWorldEventCount),
				engine_._runtimeSettings.renderDistanceChunks,
				ToDiagnosticCounter(world_metrics.pendingChunkMeshTaskCount),
				ToDiagnosticCounter(world_metrics.pendingChunkMeshUploadCount),
				ToDiagnosticCounter(world_metrics.pendingWorldGenerationTaskCount)
			});
		}
	}
