	void GameController::ProcessInput(GameplayFrameContext& frame)
	{
		const bool was_menu_open = frame.settings.isSettingsMenuOpen;
		settings_menu_controller_.ProcessInput(frame.window, frame.settings);
		if (was_menu_open || frame.settings.isSettingsMenuOpen) return;
		UpdatePlayerMovement(frame);
	}

	void GameController::UpdatePlayerMovement(GameplayFrameContext& frame)
	{
		ConsumeFlyToggle(frame.input, frame.settings, input_state_.was_fly_toggle_pressed);
		ConsumeRenderDistanceAdjustment(frame.input, frame.settings, ve::input::Key::LeftBracket, -1, input_state_.was_render_distance_decrease_pressed);
		ConsumeRenderDistanceAdjustment(frame.input, frame.settings, ve::input::Key::RightBracket, 1, input_state_.was_render_distance_increase_pressed);

		const float speed = 5.0f * static_cast<float>(frame.delta_seconds);
		const ve::gameplay::PlayerMoveIntent intent = ve::gameplay::ReadPlayerMoveIntent(frame.input);
		ve::gameplay::ApplyPlanarMovement(intent, frame.camera, speed);
		if (frame.settings.isFlying)
		{
			ve::gameplay::ApplyFlyingMovement(intent, frame.camera, speed);
		}
		else if (input_state_.is_grounded && ve::gameplay::WantsJump(intent))
		{
			frame.settings.verticalVelocity = 7.0f;
		}
		ApplyPlayerPhysics(frame);
	}

	void GameController::ApplyPlayerPhysics(GameplayFrameContext& frame)
	{
		if (frame.settings.isFlying)
		{
			frame.settings.verticalVelocity = 0.0f;
			input_state_.is_grounded = false;
			return;
		}

		constexpr float gravity_blocks_per_second = 22.0f;
		constexpr float player_eye_height = 1.7f;
		glm::vec3 position = frame.camera.GetPosition();
		frame.settings.verticalVelocity -= gravity_blocks_per_second * static_cast<float>(frame.delta_seconds);
		position.y += frame.settings.verticalVelocity * static_cast<float>(frame.delta_seconds);

		const int foot_block_x = static_cast<int>(std::floor(position.x));
		const int foot_block_y = static_cast<int>(std::floor(position.y - player_eye_height));
		const int foot_block_z = static_cast<int>(std::floor(position.z));
		if (frame.settings.verticalVelocity <= 0.0f && frame.block_registry.IsSolid(frame.world.GetBlock(foot_block_x, foot_block_y, foot_block_z)))
		{
			position.y = static_cast<float>(foot_block_y) + 1.0f + player_eye_height;
			frame.settings.verticalVelocity = 0.0f;
			input_state_.is_grounded = true;
		}
		else
		{
			input_state_.is_grounded = false;
		}

		frame.camera.MoveTo(position);
	}
