	void GameController::UpdateSelection(BlockSelectionFrameContext& frame)
	{
		constexpr float maximum_block_reach = 8.0f;
		const std::optional<ve::gameplay::BlockRaycastHit> raycast_hit =
			ve::gameplay::RaycastBlocks(
				frame.world,
				frame.block_registry,
				frame.camera.GetPosition(),
				frame.camera.GetForward(),
				maximum_block_reach);
		frame.selection.has_target = raycast_hit.has_value();
		if (raycast_hit)
		{
			frame.selection.target_block = raycast_hit->targetBlock;
			frame.selection.placement_block = raycast_hit->placementBlock;
		}
	}

	void GameController::ProcessGameplayInput(GameplayCommandFrameContext& frame)
	{
		if (frame.settings.editor.is_settings_menu_open) return;

		const auto& hotbar_blocks = ve::gameplay::DefaultHotbarBlocks();
		if (const std::optional<std::size_t> selected_hotbar_slot = ve::gameplay::ReadSelectedHotbarSlot(frame.input))
		{
			selected_placement_block_ = hotbar_blocks[*selected_hotbar_slot];
		}

		if (ve::gameplay::ConsumeDebugToggle(frame.input, input_state_.was_debug_toggle_pressed))
		{
			frame.settings.renderer.show_debug_overlay = !frame.settings.renderer.show_debug_overlay;
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
