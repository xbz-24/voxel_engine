	void GameController::UpdateSelection(GameplayFrameContext& frame)
	{
		constexpr float max_reach = 8.0f;
		const std::optional<ve::gameplay::BlockRaycastHit> hit =
			ve::gameplay::RaycastBlocks(frame.world, frame.block_registry, frame.camera.GetPosition(), frame.camera.GetForward(), max_reach);
		frame.selection.has_target = hit.has_value();
		if (hit)
		{
			frame.selection.target_block = hit->targetBlock;
			frame.selection.placement_block = hit->placementBlock;
		}
	}

	void GameController::ProcessGameplayInput(GameplayFrameContext& frame)
	{
		if (frame.settings.isSettingsMenuOpen) return;

		const auto& blocks = ve::gameplay::DefaultHotbarBlocks();
		if (const std::optional<std::size_t> selected_slot = ve::gameplay::ReadSelectedHotbarSlot(frame.input))
		{
			selected_placement_block_ = blocks[*selected_slot];
		}

		if (ve::gameplay::ConsumeDebugToggle(frame.input, input_state_.was_debug_toggle_pressed))
		{
			frame.settings.showDebugOverlay = !frame.settings.showDebugOverlay;
		}

		if (frame.selection.has_target && ve::gameplay::ConsumeBlockBreak(frame.input, input_state_.was_left_mouse_button_pressed))
		{
			ve::gameplay::BreakBlock(frame.world, frame.selection.target_block);
		}
		if (frame.selection.has_target && ve::gameplay::ConsumeBlockPlace(frame.input, input_state_.was_right_mouse_button_pressed))
		{
			ve::gameplay::PlaceBlock(frame.world, frame.selection.placement_block, selected_placement_block_);
		}
	}
