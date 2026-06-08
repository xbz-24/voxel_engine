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
		bool _wasTogglePressed = false;
		bool _wasUpPressed = false;
		bool _wasDownPressed = false;
		bool _wasLeftPressed = false;
		bool _wasRightPressed = false;
		bool _wasConfirmPressed = false;

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
		 * Activates the currently selected row.
		 */
		void Activate(Window& window, RuntimeSettings& settings);
	};
}
