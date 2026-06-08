#include "Engine.h"

#include "BlockRaycaster.h"
#include "Hotbar.h"
#include "Window.h"

#include <array>
#include <optional>

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

void Engine::ProcessGameplayInput(Window& window, ve::world::World& world, const BlockSelection& selection)
{
	if (_runtimeSettings.isSettingsMenuOpen)
	{
		return;
	}

	GLFWwindow* nativeWindow = window.GetNativeWindow();
	static constexpr std::array<int, ve::gameplay::HotbarSlotCount> hotbarKeys = {
		GLFW_KEY_1, GLFW_KEY_2, GLFW_KEY_3, GLFW_KEY_4, GLFW_KEY_5, GLFW_KEY_6, GLFW_KEY_7, GLFW_KEY_8, GLFW_KEY_9
	};

	const auto& blocks = ve::gameplay::DefaultHotbarBlocks();
	for (std::size_t index = 0; index < hotbarKeys.size(); index++)
	{
		if (glfwGetKey(nativeWindow, hotbarKeys[index]) == GLFW_PRESS)
		{
			_selectedPlacementBlock = blocks[index];
		}
	}

	const bool debugPressed = glfwGetKey(nativeWindow, GLFW_KEY_F3) == GLFW_PRESS;
	if (debugPressed && !_wasDebugTogglePressed)
	{
		_runtimeSettings.showDebugOverlay = !_runtimeSettings.showDebugOverlay;
	}
	_wasDebugTogglePressed = debugPressed;

	const bool leftPressed = glfwGetMouseButton(nativeWindow, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS;
	const bool rightPressed = glfwGetMouseButton(nativeWindow, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS;
	if (selection.hasTarget && leftPressed && !_wasLeftMouseButtonPressed)
	{
		world.SetBlock(selection.targetBlock, ve::blocks::BlockId::Air);
	}
	if (selection.hasTarget && rightPressed && !_wasRightMouseButtonPressed)
	{
		world.SetBlock(selection.placementBlock, _selectedPlacementBlock);
	}
	_wasLeftMouseButtonPressed = leftPressed;
	_wasRightMouseButtonPressed = rightPressed;
}

void Engine::UpdateGameLogic(const ve::world::World& world, const ve::blocks::BlockRegistry& blockRegistry, Camera& camera, BlockSelection& selection)
{
	performRaycastToFindTargetBlock(world, blockRegistry, camera, selection);
}
