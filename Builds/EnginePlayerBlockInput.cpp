#include "Engine.h"

#include "BlockInteraction.h"
#include "BlockRaycaster.h"
#include "GameplayInput.h"
#include "Hotbar.h"
#include "Window.h"

#include <optional>

/// Casts from the camera to find the currently selected block.
bool Engine::performRaycastToFindTargetBlock(const ve::world::World& world, const ve::blocks::BlockRegistry& blockRegistry, Camera& camera, ve::gameplay::BlockSelection& out_selection)
{
	constexpr float maxReach = 8.0f;
	const std::optional<ve::gameplay::BlockRaycastHit> hit = ve::gameplay::RaycastBlocks(world, blockRegistry, camera.GetPosition(), camera.GetForward(), maxReach);
	out_selection.has_target = hit.has_value();
	if (hit)
	{
		out_selection.target_block = hit->targetBlock;
		out_selection.placement_block = hit->placementBlock;
	}
	return out_selection.has_target;
}

/// Applies hotbar, debug, and block interaction input for one frame.
void Engine::ProcessGameplayInput(Window& window, ve::world::World& world, const ve::gameplay::BlockSelection& selection)
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

	if (ve::gameplay::ConsumeDebugToggle(nativeWindow, _input_state.was_debug_toggle_pressed))
	{
		_runtimeSettings.showDebugOverlay = !_runtimeSettings.showDebugOverlay;
	}

	if (selection.has_target && ve::gameplay::ConsumeBlockBreak(nativeWindow, _input_state.was_left_mouse_button_pressed))
	{
		ve::gameplay::BreakBlock(world, selection.target_block);
	}
	if (selection.has_target && ve::gameplay::ConsumeBlockPlace(nativeWindow, _input_state.was_right_mouse_button_pressed))
	{
		ve::gameplay::PlaceBlock(world, selection.placement_block, _selectedPlacementBlock);
	}
}

/// Updates target selection for the current frame.
void Engine::UpdateGameLogic(const ve::world::World& world, const ve::blocks::BlockRegistry& blockRegistry, Camera& camera, ve::gameplay::BlockSelection& selection)
{
	performRaycastToFindTargetBlock(world, blockRegistry, camera, selection);
}
