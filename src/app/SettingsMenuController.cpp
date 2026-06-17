#include "SettingsMenuController.h"

#include "Input.h"

namespace ve::gameplay
{
	/// Processes one frame of settings menu input.
	void SettingsMenuController::ProcessInput(ve::engine::Window& window, RuntimeSettings& settings)
	{
		ToggleMenuFromInput(window, settings);
		if (settings.isSettingsMenuOpen)
		{
			ProcessOpenMenuInput(window, settings);
		}
	}

	/// Opens or closes the menu when the toggle key is pressed.
	void SettingsMenuController::ToggleMenuFromInput(ve::engine::Window& window, RuntimeSettings& settings)
	{
		if (ve::input::WasPressed(window.GetNativeWindow(), ve::input::Key::Escape, was_toggle_pressed_))
		{
			SetOpen(window, settings, !settings.isSettingsMenuOpen);
		}
	}

	/// Processes navigation and activation while the menu is open.
	void SettingsMenuController::ProcessOpenMenuInput(ve::engine::Window& window, RuntimeSettings& settings)
	{
		GLFWwindow* nativeWindow = window.GetNativeWindow();
		if (ve::input::WasPressed(nativeWindow, ve::input::Key::Up, was_up_pressed_)) MoveSelection(settings, -1);
		if (ve::input::WasPressed(nativeWindow, ve::input::Key::Down, was_down_pressed_)) MoveSelection(settings, 1);
		if (ve::input::WasPressed(nativeWindow, ve::input::Key::Left, was_left_pressed_)) ApplyAdjustment(window, settings, -1);
		if (ve::input::WasPressed(nativeWindow, ve::input::Key::Right, was_right_pressed_)) ApplyAdjustment(window, settings, 1);
		if (ve::input::WasPressed(nativeWindow, ve::input::Key::Enter, was_confirm_pressed_)) Activate(window, settings);
	}

	/// Opens or closes the settings menu and updates cursor mode.
	void SettingsMenuController::SetOpen(ve::engine::Window& window, RuntimeSettings& settings, bool isOpen)
	{
		settings.isSettingsMenuOpen = isOpen;
		window.SetCursorMode(isOpen ? ve::engine::Window::CursorMode::Normal : ve::engine::Window::CursorMode::Captured);
	}

	/// Moves the selected menu row.
	void SettingsMenuController::MoveSelection(RuntimeSettings& settings, int direction)
	{
		const int count = static_cast<int>(ve::ui::SettingsMenuOption::Count);
		const int selected = static_cast<int>(settings.selectedSettingsMenuOption);
		settings.selectedSettingsMenuOption = static_cast<ve::ui::SettingsMenuOption>((selected + direction + count) % count);
	}

	/// Applies left/right changes to configurable rows.
	void SettingsMenuController::ApplyAdjustment(ve::engine::Window& window, RuntimeSettings& settings, int direction)
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
	void SettingsMenuController::Activate(ve::engine::Window& window, RuntimeSettings& settings)
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
