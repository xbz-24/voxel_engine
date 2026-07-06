	/** Updates gameplay systems that still need the OpenGL block registry. */
	void EngineRuntime::UpdateGameplay()
	{
		ve::blocks::BlockRegistry* block_registry = model_->MutableBlockRegistry();
		assert(block_registry != nullptr);
		controller_.Update(window_, *model_, *block_registry, engine_._runtimeSettings, frame_timer_.DeltaSeconds());
	}

	/** Renders the voxel world through the OpenGL compatibility path. */
	void EngineRuntime::RenderWorld(RenderView& render_view)
	{
		ve::blocks::BlockRegistry* block_registry = model_->MutableBlockRegistry();
		assert(block_registry != nullptr);
		SkyBox* skybox = render_view.Skybox();
		Plane* ground_plane = render_view.GroundPlane();
		BlockSelectionCube* selection_cube = render_view.SelectionCube();
		assert(skybox != nullptr);
		assert(ground_plane != nullptr);
		assert(selection_cube != nullptr);
		engine_.Render3DWorld(render_view, window_, model_->MutableCamera(), *skybox, *ground_plane,
			*selection_cube, *block_registry, model_->MutableWorld(), model_->GetSelection());
	}

	/** Renders the HUD through the OpenGL compatibility path. */
	void EngineRuntime::RenderHud(RenderView& render_view)
	{
		// TODO: Introduce a backend-neutral HUD renderer so Vulkan and OpenGL share the same HudFrameInfo pipeline.
		const ve::blocks::BlockRegistry* block_registry = model_->GetBlockRegistry();
		assert(block_registry != nullptr);
		ve::ui::HudRenderer* hud = render_view.Hud();
		assert(hud != nullptr);
		hud->Draw(engine_.CreateHudFrame(window_, model_->GetCamera(), frame_timer_,
			model_->GetSelection(), *block_registry, model_->GetWorld(), controller_.SelectedPlacementBlock()));
	}
