	/// Updates player input, targeting, physics, and block edits.
	void GameController::Update(Window& window,
		GameModel& model,
		const ve::blocks::BlockRegistry& block_registry,
		ve::gameplay::RuntimeSettings& settings,
		double delta_seconds)
	{
		(void)model.PumpAsyncWorldGeneration();
		model.PumpAsyncChunkMeshing(block_registry, settings.renderDistanceChunks);
		const ve::input::InputSnapshot input = ve::input::CaptureInputSnapshot(window);
		GameplayFrameContext frame(
			window,
			input,
			model.MutableWorld(),
			block_registry,
			model.MutableCamera(),
			model.MutableSelection(),
			settings,
			delta_seconds
		);
		UpdateFrameGameplay(frame);
	}

	ve::blocks::BlockId GameController::SelectedPlacementBlock() const noexcept
	{
		return selected_placement_block_;
	}

	void GameController::UpdateFrameGameplay(GameplayFrameContext& frame)
	{
		ProcessInput(frame);
		if (frame.settings.isSettingsMenuOpen)
		{
			frame.selection.has_target = false;
			return;
		}
		UpdateSelection(frame);
		ProcessGameplayInput(frame);
		UpdateSelection(frame);
	}
