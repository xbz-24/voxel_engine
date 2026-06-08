#include "SettingsMenuController.h"

#include <algorithm>

namespace
{
	/**
	 * Converts a key press into a one-shot action.
	 */
	bool ConsumePress(GLFWwindow* window, int key, bool& wasPressed)
	{
		const bool isPressed = glfwGetKey(window, key) == GLFW_PRESS;
		const bool wasJustPressed = isPressed && !wasPressed;
		wasPressed = isPressed;
		return wasJustPressed;
	}
}

namespace ve::gameplay
{
	void SettingsMenuController::ProcessInput(Window& window, RuntimeSettings& settings)
	{
		GLFWwindow* nativeWindow = window.GetNativeWindow();
		if (ConsumePress(nativeWindow, GLFW_KEY_ESCAPE, _wasTogglePressed))
		{
			SetOpen(window, settings, !settings.isSettingsMenuOpen);
		}
		if (!settings.isSettingsMenuOpen)
		{
			return;
		}

		if (ConsumePress(nativeWindow, GLFW_KEY_UP, _wasUpPressed)) MoveSelection(settings, -1);
		if (ConsumePress(nativeWindow, GLFW_KEY_DOWN, _wasDownPressed)) MoveSelection(settings, 1);
		if (ConsumePress(nativeWindow, GLFW_KEY_LEFT, _wasLeftPressed)) ApplyAdjustment(window, settings, -1);
		if (ConsumePress(nativeWindow, GLFW_KEY_RIGHT, _wasRightPressed)) ApplyAdjustment(window, settings, 1);
		if (ConsumePress(nativeWindow, GLFW_KEY_ENTER, _wasConfirmPressed)) Activate(window, settings);
	}

	void SettingsMenuController::SetOpen(Window& window, RuntimeSettings& settings, bool isOpen)
	{
		settings.isSettingsMenuOpen = isOpen;
		glfwSetInputMode(window.GetNativeWindow(), GLFW_CURSOR, isOpen ? GLFW_CURSOR_NORMAL : GLFW_CURSOR_DISABLED);
	}

	void SettingsMenuController::MoveSelection(RuntimeSettings& settings, int direction)
	{
		const int count = static_cast<int>(ve::ui::SettingsMenuOption::Count);
		const int selected = static_cast<int>(settings.selectedSettingsMenuOption);
		settings.selectedSettingsMenuOption = static_cast<ve::ui::SettingsMenuOption>((selected + direction + count) % count);
	}

	void SettingsMenuController::ApplyAdjustment(Window& window, RuntimeSettings& settings, int direction)
	{
		switch (settings.selectedSettingsMenuOption)
		{
		case ve::ui::SettingsMenuOption::RenderDistance:
			settings.renderDistanceChunks = std::clamp(settings.renderDistanceChunks + direction, 1, 6);
			break;
		case ve::ui::SettingsMenuOption::VSync:
			settings.isVSyncEnabled = !settings.isVSyncEnabled;
			window.SetVSync(settings.isVSyncEnabled);
			break;
		case ve::ui::SettingsMenuOption::DebugOverlay:
			settings.showDebugOverlay = !settings.showDebugOverlay;
			break;
		case ve::ui::SettingsMenuOption::FlyMode:
			settings.isFlying = !settings.isFlying;
			settings.verticalVelocity = 0.0f;
			break;
		case ve::ui::SettingsMenuOption::Resume:
		case ve::ui::SettingsMenuOption::Quit:
		case ve::ui::SettingsMenuOption::Count:
			break;
		}
	}

	void SettingsMenuController::Activate(Window& window, RuntimeSettings& settings)
	{
		if (settings.selectedSettingsMenuOption == ve::ui::SettingsMenuOption::Resume)
		{
			SetOpen(window, settings, false);
		}
		else if (settings.selectedSettingsMenuOption == ve::ui::SettingsMenuOption::Quit)
		{
			window.Close();
		}
		else
		{
			ApplyAdjustment(window, settings, 1);
		}
	}
}
