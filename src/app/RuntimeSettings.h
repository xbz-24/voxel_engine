#pragma once

#include "RenderApi.h"
#include "RenderDistanceSettings.h"
#include "SettingsMenu.h"

namespace ve::gameplay
{
	/**
	 * Runtime camera-look controls shared by GLFW callbacks and future public camera settings.
	 */
	struct MouseLookSettings
	{
		float sensitivity_degrees_per_pixel = 0.1f;
		bool invert_x_axis = false;
		bool invert_y_axis = false;
	};

	/**
	 * Mutable runtime options shared by input, render, HUD, and gameplay systems.
	 */
	struct RuntimeSettings
	{
		// TODO: Separate player/gameplay state from renderer/editor settings so public runtime config can be saved cleanly.
		bool isSettingsMenuOpen = false;
		bool isVSyncEnabled = false;
		bool showDebugOverlay = true;
		bool isFlying = false;
		float verticalVelocity = 0.0f;
		MouseLookSettings mouse_look{};
		int renderDistanceChunks = DefaultRenderDistanceChunks;
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
