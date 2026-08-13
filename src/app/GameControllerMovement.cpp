#include "GameController.h"
#include "GameControllerHelpers.h"

#include "BlockInteraction.h"
#include "BlockRegistry.h"
#include "BlockRaycaster.h"
#include "CoreTypes.h"
#include "GameplayInput.h"
#include "Hotbar.h"
#include "Input.h"
#include "PlayerMovementInput.h"
#include "RenderDistanceSettings.h"
#include "WorldGridMath.h"

#include <algorithm>
#include <array>
#include <cmath>
#include <optional>

namespace ve::engine
{
void GameController::ProcessInput(Window& window, PlayerMovementFrameContext& movement_frame)
	{
		const bool was_menu_open = movement_frame.settings.editor.is_settings_menu_open;
		settings_menu_controller_.ProcessInput(window, movement_frame.settings);
		if (was_menu_open || movement_frame.settings.editor.is_settings_menu_open) return;
		UpdatePlayerMovement(movement_frame);
	}

	void GameController::UpdatePlayerMovement(PlayerMovementFrameContext& frame)
	{
		game_controller_detail::ConsumeFlyToggle(frame.input, frame.settings, input_state_.was_fly_toggle_pressed);
		game_controller_detail::ConsumeRenderDistanceAdjustment(frame.input,
			frame.settings,
			ve::gameplay::GameplayAction::RenderDistanceDecrease,
			-1,
			input_state_.was_render_distance_decrease_pressed);
		game_controller_detail::ConsumeRenderDistanceAdjustment(frame.input,
			frame.settings,
			ve::gameplay::GameplayAction::RenderDistanceIncrease,
			1,
			input_state_.was_render_distance_increase_pressed);

		const float movement_distance_this_frame = 5.0f * ve::core::ToFloat(frame.delta_seconds);
		const ve::gameplay::PlayerMoveIntent intent = ve::gameplay::ReadPlayerMoveIntent(frame.input);
		ve::gameplay::ApplyPlanarMovement(intent, frame.camera, movement_distance_this_frame);
		if (frame.settings.player.is_flying)
		{
			ve::gameplay::ApplyFlyingMovement(intent, frame.camera, movement_distance_this_frame);
		}
		else if (input_state_.is_grounded && ve::gameplay::WantsJump(intent))
		{
			frame.settings.player.vertical_velocity = 7.0f;
		}
		ApplyPlayerPhysics(frame);
	}

	void GameController::ApplyPlayerPhysics(PlayerMovementFrameContext& frame)
	{
		if (frame.settings.player.is_flying)
		{
			frame.settings.player.vertical_velocity = 0.0f;
			input_state_.is_grounded = false;
			return;
		}

		constexpr float gravity_blocks_per_second = 22.0f;
		constexpr float player_eye_height = 1.7f;
		glm::vec3 position = frame.camera.GetPosition();
		frame.settings.player.vertical_velocity -= gravity_blocks_per_second * ve::core::ToFloat(frame.delta_seconds);
		position.y += frame.settings.player.vertical_velocity * ve::core::ToFloat(frame.delta_seconds);

		const int player_feet_block_x = ve::world::grid::BlockCoordinateFromWorld(position.x);
		const int player_feet_block_y = ve::world::grid::BlockCoordinateFromWorld(position.y - player_eye_height);
		const int player_feet_block_z = ve::world::grid::BlockCoordinateFromWorld(position.z);
		const ve::blocks::BlockId block_below_player =
			frame.world.GetBlock(player_feet_block_x, player_feet_block_y, player_feet_block_z);
		if (frame.settings.player.vertical_velocity <= 0.0f && frame.block_registry.IsSolid(block_below_player))
		{
			position.y = ve::core::ToFloat(player_feet_block_y) + 1.0f + player_eye_height;
			frame.settings.player.vertical_velocity = 0.0f;
			input_state_.is_grounded = true;
		}
		else
		{
			input_state_.is_grounded = false;
		}

		frame.camera.MoveTo(position);
	}
}
