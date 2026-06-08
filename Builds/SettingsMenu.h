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
		bool isOpen;
		SettingsMenuOption selectedOption;
		int renderDistanceChunks;
		bool showDebugOverlay;
		bool isFlying;
	};
}
