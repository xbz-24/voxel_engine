	namespace
	{
		[[nodiscard]] int ToDiagnosticCounter(std::size_t value) noexcept
		{
			return value > ve::core::ToIndex(std::numeric_limits<int>::max())
				? std::numeric_limits<int>::max()
				: ve::core::ToInt(value);
		}

		[[nodiscard]] RuntimeFrameContext BuildRuntimeFrameContextForPublicUpdate(
			const ve::time::FrameTimer& frame_timer,
			double elapsed_seconds,
			const RuntimeInputSnapshot& runtime_input_snapshot)
		{
			RuntimeFrameContext runtime_frame_context{};
			runtime_frame_context.delta_seconds = ve::core::ToFloat(frame_timer.DeltaSeconds());
			runtime_frame_context.elapsed_seconds = ve::core::ToFloat(elapsed_seconds);
			runtime_frame_context.fps = frame_timer.DisplayedFps();
			runtime_frame_context.input = runtime_input_snapshot;
			return runtime_frame_context;
		}

		void PopulateRuntimeFrameCameraState(RuntimeFrameContext& runtime_frame_context,
			const Camera& camera)
		{
			runtime_frame_context.camera.position = camera.GetPosition();
			runtime_frame_context.camera.forward = camera.GetForward();
		}

		void PopulateRuntimeFrameSelectionState(RuntimeFrameContext& runtime_frame_context,
			const GameModel& model,
			const GameController& controller)
		{
			runtime_frame_context.selected_block = controller.SelectedPlacementBlock();

			const ve::gameplay::BlockSelection& selection = model.GetSelection();
			runtime_frame_context.hit_result.has_hit = selection.has_target;
			runtime_frame_context.hit_result.target_block = selection.target_block;
			runtime_frame_context.hit_result.placement_block = selection.placement_block;
			if (selection.has_target)
			{
				runtime_frame_context.hit_result.target_block_id = model.GetWorld().GetBlock(selection.target_block);
			}
		}

		void PopulateRuntimeFrameModelState(RuntimeFrameContext& runtime_frame_context,
			const GameModel& model,
			const GameController& controller)
		{
			PopulateRuntimeFrameCameraState(runtime_frame_context, model.GetCamera());
			PopulateRuntimeFrameSelectionState(runtime_frame_context, model, controller);
		}

		[[nodiscard]] RuntimeDiagnostics BuildRuntimeDiagnosticsSnapshot(
			const ve::time::FrameTimer& frame_timer,
			const ve::world::WorldMetrics& world_metrics,
			int render_distance_chunks)
		{
			return RuntimeDiagnostics{
				ve::core::ToDouble(frame_timer.DisplayedFps()),
				ToDiagnosticCounter(world_metrics.pendingWorldEventCount),
				render_distance_chunks,
				ToDiagnosticCounter(world_metrics.pendingChunkMeshTaskCount),
				ToDiagnosticCounter(world_metrics.pendingChunkMeshUploadCount),
				ToDiagnosticCounter(world_metrics.pendingWorldGenerationTaskCount)
			};
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

		const RuntimeInputSnapshot runtime_input_snapshot = CaptureRuntimeInputSnapshot(window_);

