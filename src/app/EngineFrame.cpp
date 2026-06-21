#include "Engine.h"

/// Builds immutable HUD data for the current frame.
ve::ui::HudFrameInfo EngineApplication::CreateHudFrame(const ve::engine::Window& window,
	const Camera& camera,
	const ve::time::FrameTimer& frameTimer,
	const ve::gameplay::BlockSelection& selection,
	const ve::blocks::BlockRegistry& blockRegistry,
	const ve::world::World& world,
	ve::blocks::BlockId selectedPlacementBlock) const
{
	return ve::ui::HudFrameInfo{ window, camera, frameTimer.DisplayedFps(), selection.target_block, selection.has_target, blockRegistry, selectedPlacementBlock, _runtimeSettings.showDebugOverlay, _runtimeSettings.isFlying, _runtimeSettings.renderDistanceChunks, world.PendingEventCount(), ve::gameplay::ToSettingsMenuState(_runtimeSettings) };
}
