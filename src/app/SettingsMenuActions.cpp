#include "SettingsMenuController.h"

namespace ve::gameplay
{
	/// Adjusts render distance within the supported range.
	void SettingsMenuController::AdjustRenderDistance(RuntimeSettings& settings, int direction)
	{
		settings.renderer.render_distance_chunks = ClampRenderDistanceChunks(settings.renderer.render_distance_chunks + direction);
	}

	/// Toggles VSync and applies it to the native window.
	void SettingsMenuController::ToggleVSync(ve::engine::Window& window, RuntimeSettings& settings)
	{
		settings.renderer.is_vsync_enabled = !settings.renderer.is_vsync_enabled;
		window.SetVSync(settings.renderer.is_vsync_enabled);
	}

	/// Toggles debug overlay visibility.
	void SettingsMenuController::ToggleDebugOverlay(RuntimeSettings& settings)
	{
		settings.renderer.show_debug_overlay = !settings.renderer.show_debug_overlay;
	}

	/// Toggles fly mode and clears vertical velocity.
	void SettingsMenuController::ToggleFlyMode(RuntimeSettings& settings)
	{
		settings.player.is_flying = !settings.player.is_flying;
		settings.player.vertical_velocity = 0.0f;
	}
}
