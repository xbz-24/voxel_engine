	/// Updates player input, targeting, physics, and block edits.
	void GameController::Update(Window& window,
		GameModel& model,
		const ve::blocks::BlockRegistry& block_registry,
		ve::gameplay::RuntimeSettings& settings,
		double delta_seconds)
	{
		(void)model.PumpAsyncWorldGeneration();
		model.PumpAsyncChunkMeshing(block_registry, settings.renderer.render_distance_chunks);
		const ve::input::InputSnapshot input = ve::input::CaptureInputSnapshot(window);
		UpdateFrameGameplay(
			window,
			input,
			model.MutableWorld(),
			block_registry,
			model.MutableCamera(),
			model.MutableSelection(),
			settings,
			delta_seconds
		);
	}

	ve::blocks::BlockId GameController::SelectedPlacementBlock() const noexcept
	{
		return selected_placement_block_;
	}

	void GameController::UpdateFrameGameplay(
		Window& window,
		const ve::input::InputSnapshot& input,
		ve::world::World& world,
		const ve::blocks::BlockRegistry& block_registry,
		Camera& camera,
		ve::gameplay::BlockSelection& selection,
		ve::gameplay::RuntimeSettings& settings,
		double delta_seconds)
	{
		PlayerMovementFrameContext movement_frame(input, world, block_registry, camera, settings, delta_seconds);
		ProcessInput(window, movement_frame);
		if (settings.editor.is_settings_menu_open)
		{
			selection.has_target = false;
			return;
		}

		BlockSelectionFrameContext selection_frame(world, block_registry, camera, selection);
		GameplayCommandFrameContext command_frame(input, world, selection, settings);
		UpdateSelection(selection_frame);
		ProcessGameplayInput(command_frame);
		UpdateSelection(selection_frame);
	}
