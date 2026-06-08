#include "Engine.h"

#include "BlockInteraction.h"
#include "BlockRaycaster.h"
#include "GameplayInput.h"
#include "Hotbar.h"
#include "Window.h"

#include <optional>

/// Casts from the camera to find the currently selected block.
bool Engine::performRaycastToFindTargetBlock(const ve::world::World& world, const ve::blocks::BlockRegistry& blockRegistry, Camera& camera, BlockSelection& out_selection)
{
	constexpr float maxReach = 8.0f;
	const std::optional<ve::gameplay::BlockRaycastHit> hit = ve::gameplay::RaycastBlocks(world, blockRegistry, camera.GetPosition(), camera.GetForward(), maxReach);
	out_selection.hasTarget = hit.has_value();
	if (hit)
	{
		out_selection.targetBlock = hit->targetBlock;
		out_selection.placementBlock = hit->placementBlock;
	}
	return out_selection.hasTarget;
}

/// Applies hotbar, debug, and block interaction input for one frame.
void Engine::ProcessGameplayInput(Window& window, ve::world::World& world, const BlockSelection& selection)
{
	if (_runtimeSettings.isSettingsMenuOpen)
	{
		return;
	}

	GLFWwindow* nativeWindow = window.GetNativeWindow();
	const auto& blocks = ve::gameplay::DefaultHotbarBlocks();
	if (const std::optional<std::size_t> selectedSlot = ve::gameplay::ReadSelectedHotbarSlot(nativeWindow))
	{
		_selectedPlacementBlock = blocks[*selectedSlot];
	}

	if (ve::gameplay::ConsumeDebugToggle(nativeWindow, _wasDebugTogglePressed))
	{
		_runtimeSettings.showDebugOverlay = !_runtimeSettings.showDebugOverlay;
	}

	if (selection.hasTarget && ve::gameplay::ConsumeBlockBreak(nativeWindow, _wasLeftMouseButtonPressed))
	{
		ve::gameplay::BreakBlock(world, selection.targetBlock);
	}
	if (selection.hasTarget && ve::gameplay::ConsumeBlockPlace(nativeWindow, _wasRightMouseButtonPressed))
	{
		ve::gameplay::PlaceBlock(world, selection.placementBlock, _selectedPlacementBlock);
	}
}

/// Updates target selection for the current frame.
void Engine::UpdateGameLogic(const ve::world::World& world, const ve::blocks::BlockRegistry& blockRegistry, Camera& camera, BlockSelection& selection)
{
	performRaycastToFindTargetBlock(world, blockRegistry, camera, selection);
}
