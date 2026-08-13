#pragma once

namespace ve::time
{
	class FrameTimer;
}

namespace ve::world
{
	struct WorldMetrics;
}

namespace ve::engine
{
	class GameController;
	class GameModel;
	struct RuntimeDiagnostics;
	struct RuntimeFrameContext;
	struct RuntimeInputSnapshot;
}

namespace ve::engine::runtime_frame_snapshot
{
	[[nodiscard]] RuntimeFrameContext BuildFrameContext(
		const ve::time::FrameTimer& frame_timer,
		double elapsed_seconds,
		const RuntimeInputSnapshot& input_snapshot);
	void PopulateModelState(
		RuntimeFrameContext& frame_context,
		const GameModel& model,
		const GameController& controller);
	[[nodiscard]] RuntimeDiagnostics BuildDiagnostics(
		const ve::time::FrameTimer& frame_timer,
		const ve::world::WorldMetrics& world_metrics,
		int render_distance_chunks);
}
