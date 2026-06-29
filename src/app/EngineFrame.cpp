#include "Engine.h"

/// Builds immutable HUD data for the current frame.
ve::ui::HudFrameInfo EngineApplication::CreateHudFrame(const ve::engine::Window& window,
	const Camera& camera,
	const ve::time::FrameTimer& frame_timer,
	const ve::gameplay::BlockSelection& selection,
	const ve::blocks::BlockRegistry& block_registry,
	const ve::world::World& world,
	ve::blocks::BlockId selected_placement_block) const
{
	return ve::ui::HudFrameInfo{
		window,
		camera,
		frame_timer.DisplayedFps(),
		selection.target_block,
		selection.has_target,
		block_registry,
		selected_placement_block,
		_runtimeSettings.showDebugOverlay,
		_runtimeSettings.isFlying,
		_runtimeSettings.renderDistanceChunks,
		world.PendingEventCount(),
		ve::gameplay::ToSettingsMenuState(_runtimeSettings)
	};
}
