#pragma once

#include <cstdint>

namespace ve::ui
{
	/**
	 * Identifies one selectable row in the in-game settings menu.
	 */
	enum class SettingsMenuOption : std::uint8_t
	{
		RenderDistance,
		VSync,
		DebugOverlay,
		FlyMode,
		Resume,
		Quit,
		Count
	};

	/**
	 * Read-only state needed to render the settings menu.
	 */
	struct SettingsMenuState
	{
		bool is_open;
		SettingsMenuOption selected_option;
		int render_distance_chunks;
		bool is_vsync_enabled;
		bool show_debug_overlay;
		bool is_flying;
	};
}
