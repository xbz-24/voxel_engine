#include "RuntimeSettings.h"

namespace ve::gameplay
{
	ve::ui::SettingsMenuState ToSettingsMenuState(const RuntimeSettings& settings)
	{
		return ve::ui::SettingsMenuState{
			settings.isSettingsMenuOpen,
			settings.selectedSettingsMenuOption,
			settings.renderDistanceChunks,
			settings.isVSyncEnabled,
			settings.showDebugOverlay,
			settings.isFlying
		};
	}
}
