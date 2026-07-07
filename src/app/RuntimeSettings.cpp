#include "RuntimeSettings.h"

namespace ve::gameplay
{
	ve::ui::SettingsMenuState ToSettingsMenuState(const RuntimeSettings& settings)
	{
		return ve::ui::SettingsMenuState{
			settings.editor.is_settings_menu_open,
			settings.editor.selected_settings_menu_option,
			settings.renderer.render_distance_chunks,
			settings.renderer.is_vsync_enabled,
			settings.renderer.show_debug_overlay,
			settings.player.is_flying
		};
	}
}
