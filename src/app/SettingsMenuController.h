#pragma once

#include "RuntimeSettings.h"
#include "Window.h"

namespace ve::gameplay
{
	/**
	 * Handles keyboard navigation and mutations for the settings menu.
	 */
	class SettingsMenuController
	{
	public:
		/**
		 * Processes one frame of settings menu input.
		 *
		 * @param window Window used for input, cursor state, VSync, and quit.
		 * @param settings Runtime settings mutated by menu rows.
		 */
		void ProcessInput(Window& window, RuntimeSettings& settings);

	private:
		bool was_toggle_pressed_ = false;
		bool was_up_pressed_ = false;
		bool was_down_pressed_ = false;
		bool was_left_pressed_ = false;
		bool was_right_pressed_ = false;
		bool was_confirm_pressed_ = false;

		/**
		 * Opens or closes the menu when the toggle key is pressed.
		 */
		void ToggleMenuFromInput(Window& window, RuntimeSettings& settings);

		/**
		 * Processes navigation and activation while the menu is open.
		 */
		void ProcessOpenMenuInput(Window& window, RuntimeSettings& settings);

		/**
		 * Opens or closes the settings menu and updates cursor mode.
		 */
		void SetOpen(Window& window, RuntimeSettings& settings, bool isOpen);

		/**
		 * Moves the selected menu row.
		 */
		void MoveSelection(RuntimeSettings& settings, int direction);

		/**
		 * Applies left/right changes to configurable rows.
		 */
		void ApplyAdjustment(Window& window, RuntimeSettings& settings, int direction);

		/**
		 * Adjusts render distance within the supported range.
		 */
		void AdjustRenderDistance(RuntimeSettings& settings, int direction);

		/**
		 * Toggles VSync and applies it to the native window.
		 */
		void ToggleVSync(Window& window, RuntimeSettings& settings);

		/**
		 * Toggles debug overlay visibility.
		 */
		void ToggleDebugOverlay(RuntimeSettings& settings);

		/**
		 * Toggles fly mode and clears vertical velocity.
		 */
		void ToggleFlyMode(RuntimeSettings& settings);

		/**
		 * Activates the currently selected row.
		 */
		void Activate(Window& window, RuntimeSettings& settings);
	};
}
