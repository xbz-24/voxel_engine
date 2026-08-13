#include "SettingsMenuController.h"

#include "CoreTypes.h"

namespace ve::gameplay
{
	void SettingsMenuController::SetOpen(
		ve::engine::Window& window,
		RuntimeSettings& settings,
		bool is_open)
	{
		settings.editor.is_settings_menu_open = is_open;
		window.SetCursorMode(is_open
			? ve::engine::Window::CursorMode::Normal
			: ve::engine::Window::CursorMode::Captured);
	}

	void SettingsMenuController::MoveSelection(RuntimeSettings& settings, int direction)
	{
		const int count = ve::core::ToInt(ve::ui::SettingsMenuOption::Count);
		const int selected = ve::core::ToInt(settings.editor.selected_settings_menu_option);
		settings.editor.selected_settings_menu_option =
			ve::core::NumericCast<ve::ui::SettingsMenuOption>(
				(selected + direction + count) % count);
	}
}
