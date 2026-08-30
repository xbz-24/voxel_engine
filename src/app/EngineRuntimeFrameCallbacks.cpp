#include "VoxelSandboxModule.h"

#include "Engine.h"
#include "EngineRuntimeFrameSnapshots.h"
#include "GameModel.h"
#include "RuntimeInput.h"
#include "Window.h"

namespace ve::engine
{
	void VoxelSandboxModule::InvokePublicApiFrameCallbacks(RuntimeModuleContext& context)
	{
		const EngineCreateInfo& create_info = engine_.CreateInfo();
		if (!create_info.on_update && !create_info.on_diagnostics) return;

		const RuntimeInputSnapshot input_snapshot = CaptureRuntimeInputSnapshot(context.window);
		if (create_info.on_update)
		{
			RuntimeFrameContext frame_context = runtime_frame_snapshot::BuildFrameContext(
				context.frame_timer, elapsed_seconds_, input_snapshot);
			if (model_ != nullptr)
			{
				runtime_frame_snapshot::PopulateModelState(frame_context, *model_, controller_);
			}
			create_info.on_update(frame_context);
			ApplyWorldEdits(frame_context.world_edits);
			if (frame_context.request_close) context.window.Close();
		}

		if (create_info.on_diagnostics)
		{
			const ve::world::WorldMetrics world_metrics =
				model_ != nullptr ? model_->GetWorldMetrics() : ve::world::WorldMetrics{};
			create_info.on_diagnostics(runtime_frame_snapshot::BuildDiagnostics(
				context.frame_timer,
				world_metrics,
				runtime_settings_.renderer.render_distance_chunks));
		}
	}
}
