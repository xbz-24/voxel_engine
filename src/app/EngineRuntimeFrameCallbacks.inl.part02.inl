		if (create_info.on_update)
		{
			RuntimeFrameContext runtime_frame_context =
				BuildRuntimeFrameContextForPublicUpdate(frame_timer_, elapsed_seconds_, runtime_input_snapshot);
			if (model_ != nullptr)
			{
				PopulateRuntimeFrameModelState(runtime_frame_context, *model_, controller_);
			}
			create_info.on_update(runtime_frame_context);
			ApplyWorldEdits(runtime_frame_context.world_edits);
			if (runtime_frame_context.request_close) window_.Close();
		}

		if (create_info.on_diagnostics)
		{
			const ve::world::WorldMetrics world_metrics =
				model_ != nullptr ? model_->GetWorldMetrics() : ve::world::WorldMetrics{};
			create_info.on_diagnostics(BuildRuntimeDiagnosticsSnapshot(
				frame_timer_,
				world_metrics,
				engine_.RuntimeSettings().renderer.render_distance_chunks));
		}
	}
