#include "Engine.h"

/// Runs input, selection and block interaction for one frame.
void Engine::UpdateFrameGameplay(ve::engine::Window& window, ve::world::World& world, const ve::blocks::BlockRegistry& blockRegistry, Camera& camera, ve::gameplay::BlockSelection& selection, double deltaSeconds)
{
	ProcessInput(window, world, blockRegistry, camera, deltaSeconds);
	if (_runtimeSettings.isSettingsMenuOpen)
	{
		selection.has_target = false;
		return;
	}
	UpdateGameLogic(world, blockRegistry, camera, selection);
	ProcessGameplayInput(window, world, selection);
	UpdateGameLogic(world, blockRegistry, camera, selection);
}

/// Builds immutable HUD data for the current frame.
ve::ui::HudFrameInfo Engine::CreateHudFrame(const ve::engine::Window& window, const Camera& camera, const ve::time::FrameTimer& frameTimer, const ve::gameplay::BlockSelection& selection, const ve::blocks::BlockRegistry& blockRegistry, const ve::world::World& world) const
{
	return ve::ui::HudFrameInfo{ window, camera, frameTimer.DisplayedFps(), selection.target_block, selection.has_target, blockRegistry, _selectedPlacementBlock, _runtimeSettings.showDebugOverlay, _runtimeSettings.isFlying, _runtimeSettings.renderDistanceChunks, world.PendingEventCount(), ve::gameplay::ToSettingsMenuState(_runtimeSettings) };
}
