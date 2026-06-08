#include "Engine.h"

/// Runs input, selection and block interaction for one frame.
void Engine::UpdateFrameGameplay(Window& window, ve::world::World& world, const ve::blocks::BlockRegistry& blockRegistry, Camera& camera, BlockSelection& selection, double deltaSeconds)
{
	ProcessInput(window, world, blockRegistry, camera, deltaSeconds);
	if (_runtimeSettings.isSettingsMenuOpen)
	{
		selection.hasTarget = false;
		return;
	}
	UpdateGameLogic(world, blockRegistry, camera, selection);
	ProcessGameplayInput(window, world, selection);
	UpdateGameLogic(world, blockRegistry, camera, selection);
}

/// Builds immutable HUD data for the current frame.
ve::ui::HudFrameInfo Engine::CreateHudFrame(const Window& window, const Camera& camera, const ve::time::FrameTimer& frameTimer, const BlockSelection& selection, const ve::blocks::BlockRegistry& blockRegistry) const
{
	return ve::ui::HudFrameInfo{ window, camera, frameTimer.DisplayedFps(), selection.targetBlock, selection.hasTarget, blockRegistry, _selectedPlacementBlock, _runtimeSettings.showDebugOverlay, _runtimeSettings.isFlying, _runtimeSettings.renderDistanceChunks, ve::gameplay::ToSettingsMenuState(_runtimeSettings) };
}
