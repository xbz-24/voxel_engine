#include "SettingsMenuController.h"

#include <algorithm>

namespace ve::gameplay
{
	/// Adjusts render distance within the supported range.
	void SettingsMenuController::AdjustRenderDistance(RuntimeSettings& settings, int direction)
	{
		settings.renderDistanceChunks = std::clamp(settings.renderDistanceChunks + direction, 1, 6);
	}

	/// Toggles VSync and applies it to the native window.
	void SettingsMenuController::ToggleVSync(Window& window, RuntimeSettings& settings)
	{
		settings.isVSyncEnabled = !settings.isVSyncEnabled;
		window.SetVSync(settings.isVSyncEnabled);
	}

	/// Toggles debug overlay visibility.
	void SettingsMenuController::ToggleDebugOverlay(RuntimeSettings& settings)
	{
		settings.showDebugOverlay = !settings.showDebugOverlay;
	}

	/// Toggles fly mode and clears vertical velocity.
	void SettingsMenuController::ToggleFlyMode(RuntimeSettings& settings)
	{
		settings.isFlying = !settings.isFlying;
		settings.verticalVelocity = 0.0f;
	}
}
