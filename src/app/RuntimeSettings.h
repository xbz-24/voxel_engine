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

	struct RuntimePlayerState
	{
		bool is_flying = false;
		float vertical_velocity = 0.0f;
		MouseLookSettings mouse_look{};
	};

	struct RuntimeRendererSettings
	{
		bool is_vsync_enabled = false;
		bool show_debug_overlay = true;
		int render_distance_chunks = DefaultRenderDistanceChunks;
		ve::rendering::RenderBackendConfiguration backend_configuration{};
	};

	struct RuntimeEditorSettings
	{
		bool settings_menu_enabled = true;
		bool is_settings_menu_open = false;
		ve::ui::SettingsMenuOption selected_settings_menu_option = ve::ui::SettingsMenuOption::RenderDistance;
	};

	/**
	 * Mutable runtime options shared by input, render, HUD, and gameplay systems.
	 */
	struct RuntimeSettings
	{
		RuntimePlayerState player{};
		RuntimeRendererSettings renderer{};
		RuntimeEditorSettings editor{};
	};

	/**
	 * Converts engine runtime settings into a read-only HUD menu state.
	 *
	 * @param settings Runtime settings to expose to UI.
	 * @return Menu state consumed by HudRenderer.
	 */
	ve::ui::SettingsMenuState ToSettingsMenuState(const RuntimeSettings& settings);
}
