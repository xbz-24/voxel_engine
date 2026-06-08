#include "SettingsMenuController.h"

#include "Input.h"

namespace ve::gameplay
{
	/// Processes one frame of settings menu input.
	void SettingsMenuController::ProcessInput(Window& window, RuntimeSettings& settings)
	{
		ToggleMenuFromInput(window, settings);
		if (settings.isSettingsMenuOpen)
		{
			ProcessOpenMenuInput(window, settings);
		}
	}

	/// Opens or closes the menu when the toggle key is pressed.
	void SettingsMenuController::ToggleMenuFromInput(Window& window, RuntimeSettings& settings)
	{
		if (ve::input::WasPressed(window.GetNativeWindow(), ve::input::Key::Escape, _wasTogglePressed))
		{
			SetOpen(window, settings, !settings.isSettingsMenuOpen);
		}
	}

	/// Processes navigation and activation while the menu is open.
	void SettingsMenuController::ProcessOpenMenuInput(Window& window, RuntimeSettings& settings)
	{
		GLFWwindow* nativeWindow = window.GetNativeWindow();
		if (ve::input::WasPressed(nativeWindow, ve::input::Key::Up, _wasUpPressed)) MoveSelection(settings, -1);
		if (ve::input::WasPressed(nativeWindow, ve::input::Key::Down, _wasDownPressed)) MoveSelection(settings, 1);
		if (ve::input::WasPressed(nativeWindow, ve::input::Key::Left, _wasLeftPressed)) ApplyAdjustment(window, settings, -1);
		if (ve::input::WasPressed(nativeWindow, ve::input::Key::Right, _wasRightPressed)) ApplyAdjustment(window, settings, 1);
		if (ve::input::WasPressed(nativeWindow, ve::input::Key::Enter, _wasConfirmPressed)) Activate(window, settings);
	}

	/// Opens or closes the settings menu and updates cursor mode.
	void SettingsMenuController::SetOpen(Window& window, RuntimeSettings& settings, bool isOpen)
	{
		settings.isSettingsMenuOpen = isOpen;
		window.SetCursorMode(isOpen ? Window::CursorMode::Normal : Window::CursorMode::Captured);
	}

	/// Moves the selected menu row.
	void SettingsMenuController::MoveSelection(RuntimeSettings& settings, int direction)
	{
		const int count = static_cast<int>(ve::ui::SettingsMenuOption::Count);
		const int selected = static_cast<int>(settings.selectedSettingsMenuOption);
		settings.selectedSettingsMenuOption = static_cast<ve::ui::SettingsMenuOption>((selected + direction + count) % count);
	}

	/// Applies left/right changes to configurable rows.
	void SettingsMenuController::ApplyAdjustment(Window& window, RuntimeSettings& settings, int direction)
	{
		switch (settings.selectedSettingsMenuOption)
		{
		case ve::ui::SettingsMenuOption::RenderDistance:
			AdjustRenderDistance(settings, direction);
			break;
		case ve::ui::SettingsMenuOption::VSync:
			ToggleVSync(window, settings);
			break;
		case ve::ui::SettingsMenuOption::DebugOverlay:
			ToggleDebugOverlay(settings);
			break;
		case ve::ui::SettingsMenuOption::FlyMode:
			ToggleFlyMode(settings);
			break;
		case ve::ui::SettingsMenuOption::Resume:
		case ve::ui::SettingsMenuOption::Quit:
		case ve::ui::SettingsMenuOption::Count:
			break;
		}
	}

	/// Activates the currently selected row.
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
