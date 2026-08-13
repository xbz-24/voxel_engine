#include "GameControllerHelpers.h"

#include "GameplayInput.h"
#include "RenderDistanceSettings.h"
#include "RuntimeSettings.h"

namespace ve::engine::game_controller_detail
{
	void ConsumeFlyToggle(
		const ve::input::InputSnapshot& input,
		ve::gameplay::RuntimeSettings& settings,
		bool& was_pressed)
	{
		if (!ve::gameplay::ConsumeGameplayAction(
			input,
			ve::gameplay::GameplayAction::ToggleFly,
			was_pressed)) return;
		settings.player.is_flying = !settings.player.is_flying;
		settings.player.vertical_velocity = 0.0f;
	}

	void ConsumeRenderDistanceAdjustment(
		const ve::input::InputSnapshot& input,
		ve::gameplay::RuntimeSettings& settings,
		ve::gameplay::GameplayAction action,
		int amount,
		bool& was_pressed)
	{
		if (!ve::gameplay::ConsumeGameplayAction(input, action, was_pressed)) return;
		settings.renderer.render_distance_chunks = ve::gameplay::ClampRenderDistanceChunks(
			settings.renderer.render_distance_chunks + amount);
	}
}
