#include "Engine.h"

#include <algorithm>

namespace
{
	/**
	 * Converts a key press into a one-shot action.
	 *
	 * @param window Native GLFW window queried for keyboard state.
	 * @param key GLFW key code.
	 * @param wasPressed Previous pressed state stored by the engine.
	 * @return true only on the frame where the key changes to pressed.
	 */
	bool ConsumePress(GLFWwindow* window, int key, bool& wasPressed)
	{
		const bool isPressed = glfwGetKey(window, key) == GLFW_PRESS;
		const bool wasJustPressed = isPressed && !wasPressed;
		wasPressed = isPressed;
		return wasJustPressed;
	}

	/**
	 * Returns the number of selectable settings rows.
	 */
	int SettingsOptionCount()
	{
		return static_cast<int>(ve::ui::SettingsMenuOption::Count);
	}
}

void Engine::ProcessSettingsMenuInput(Window& window)
{
	GLFWwindow* nativeWindow = window.GetNativeWindow();
	if (ConsumePress(nativeWindow, GLFW_KEY_ESCAPE, _wasSettingsTogglePressed))
	{
		SetSettingsMenuOpen(window, !_isSettingsMenuOpen);
	}
	if (!_isSettingsMenuOpen)
	{
		return;
	}

	if (ConsumePress(nativeWindow, GLFW_KEY_UP, _wasSettingsUpPressed)) MoveSettingsSelection(-1);
	if (ConsumePress(nativeWindow, GLFW_KEY_DOWN, _wasSettingsDownPressed)) MoveSettingsSelection(1);
	if (ConsumePress(nativeWindow, GLFW_KEY_LEFT, _wasSettingsLeftPressed)) ApplySettingsAdjustment(window, -1);
	if (ConsumePress(nativeWindow, GLFW_KEY_RIGHT, _wasSettingsRightPressed)) ApplySettingsAdjustment(window, 1);
	if (ConsumePress(nativeWindow, GLFW_KEY_ENTER, _wasSettingsConfirmPressed))
	{
		ActivateSettingsOption(window);
	}
}

void Engine::SetSettingsMenuOpen(Window& window, bool isOpen)
{
	_isSettingsMenuOpen = isOpen;
	glfwSetInputMode(window.GetNativeWindow(), GLFW_CURSOR, isOpen ? GLFW_CURSOR_NORMAL : GLFW_CURSOR_DISABLED);
}

void Engine::MoveSettingsSelection(int direction)
{
	const int count = SettingsOptionCount();
	const int selected = static_cast<int>(_selectedSettingsMenuOption);
	_selectedSettingsMenuOption = static_cast<ve::ui::SettingsMenuOption>((selected + direction + count) % count);
}

void Engine::ApplySettingsAdjustment(Window& window, int direction)
{
	switch (_selectedSettingsMenuOption)
	{
	case ve::ui::SettingsMenuOption::RenderDistance:
		_renderDistanceChunks = std::clamp(_renderDistanceChunks + direction, 1, 6);
		break;
	case ve::ui::SettingsMenuOption::DebugOverlay:
		_isDebugOverlayVisible = !_isDebugOverlayVisible;
		break;
	case ve::ui::SettingsMenuOption::FlyMode:
		_isFlying = !_isFlying;
		_verticalVelocity = 0.0f;
		break;
	case ve::ui::SettingsMenuOption::Resume:
		SetSettingsMenuOpen(window, false);
		break;
	case ve::ui::SettingsMenuOption::Quit:
		glfwSetWindowShouldClose(window.GetNativeWindow(), true);
		break;
	case ve::ui::SettingsMenuOption::Count:
		break;
	}
}

void Engine::ActivateSettingsOption(Window& window)
{
	ApplySettingsAdjustment(window, 1);
}
