#include "EngineRuntimeFrameSnapshots.h"

#include "CoreTypes.h"
#include "EngineRuntimeCallbacks.h"
#include "FrameTimer.h"
#include "GameController.h"
#include "GameModel.h"

#include <limits>

namespace ve::engine::runtime_frame_snapshot
{
	namespace
	{
		[[nodiscard]] int ToDiagnosticCounter(std::size_t value) noexcept
		{
			return value > ve::core::ToIndex(std::numeric_limits<int>::max())
				? std::numeric_limits<int>::max()
				: ve::core::ToInt(value);
		}
	}

	RuntimeFrameContext BuildFrameContext(
		const ve::time::FrameTimer& frame_timer,
		double elapsed_seconds,
		const RuntimeInputSnapshot& input_snapshot)
	{
		RuntimeFrameContext frame_context{};
		frame_context.delta_seconds = ve::core::ToFloat(frame_timer.DeltaSeconds());
		frame_context.elapsed_seconds = ve::core::ToFloat(elapsed_seconds);
		frame_context.fps = frame_timer.DisplayedFps();
		frame_context.input = input_snapshot;
		return frame_context;
	}

	void PopulateModelState(
		RuntimeFrameContext& frame_context,
		const GameModel& model,
		const GameController& controller)
	{
		frame_context.camera.position = model.GetCamera().GetPosition();
		frame_context.camera.forward = model.GetCamera().GetForward();
		frame_context.selected_block = controller.SelectedPlacementBlock();

		const ve::gameplay::BlockSelection& selection = model.GetSelection();
		frame_context.hit_result.has_hit = selection.has_target;
		frame_context.hit_result.target_block = selection.target_block;
		frame_context.hit_result.placement_block = selection.placement_block;
		if (selection.has_target)
		{
			frame_context.hit_result.target_block_id = model.GetWorld().GetBlock(selection.target_block);
		}
	}

	RuntimeDiagnostics BuildDiagnostics(
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
