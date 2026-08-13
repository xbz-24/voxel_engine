#include "EngineRuntime.h"
#include "EngineRuntimeFrameSnapshots.h"

namespace ve::engine
{
	void EngineRuntime::BeginRuntimeFrame()
	{
		frame_timer_.Tick();
		elapsed_seconds_ += frame_timer_.DeltaSeconds();
	}

	void EngineRuntime::InvokePublicApiFrameCallbacks()
	{
		const EngineCreateInfo& create_info = engine_.CreateInfo();
		if (!create_info.on_update && !create_info.on_diagnostics) return;

		const RuntimeInputSnapshot input_snapshot = CaptureRuntimeInputSnapshot(window_);
		if (create_info.on_update)
		{
			RuntimeFrameContext frame_context = runtime_frame_snapshot::BuildFrameContext(
				frame_timer_, elapsed_seconds_, input_snapshot);
			if (model_ != nullptr)
			{
				runtime_frame_snapshot::PopulateModelState(frame_context, *model_, controller_);
			}
			create_info.on_update(frame_context);
			ApplyWorldEdits(frame_context.world_edits);
			if (frame_context.request_close) window_.Close();
		}

		if (create_info.on_diagnostics)
		{
			const ve::world::WorldMetrics world_metrics =
				model_ != nullptr ? model_->GetWorldMetrics() : ve::world::WorldMetrics{};
			create_info.on_diagnostics(runtime_frame_snapshot::BuildDiagnostics(
				frame_timer_,
				world_metrics,
				engine_.RuntimeSettings().renderer.render_distance_chunks));
		}
	}
}
