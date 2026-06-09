#pragma once

#include "RenderApi.h"
#include "SettingsMenu.h"

namespace ve::gameplay
{
	/**
	 * Mutable runtime options shared by input, render, HUD, and gameplay systems.
	 */
	struct RuntimeSettings
	{
		bool isSettingsMenuOpen = false;
		bool isVSyncEnabled = false;
		bool showDebugOverlay = true;
		bool isFlying = false;
		float verticalVelocity = 0.0f;
		int renderDistanceChunks = 2;
		ve::rendering::RenderBackendConfiguration renderBackendConfiguration{};
		ve::ui::SettingsMenuOption selectedSettingsMenuOption = ve::ui::SettingsMenuOption::RenderDistance;
	};

	/**
	 * Converts engine runtime settings into a read-only HUD menu state.
	 *
	 * @param settings Runtime settings to expose to UI.
	 * @return Menu state consumed by HudRenderer.
	 */
	ve::ui::SettingsMenuState ToSettingsMenuState(const RuntimeSettings& settings);
}
