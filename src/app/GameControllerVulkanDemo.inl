	/// Updates the temporary Vulkan migration demo.
	void GameController::UpdateVulkanDemo(Window& window,
		GameModel& model,
		const ve::blocks::BlockRegistry& block_registry,
		ve::gameplay::RuntimeSettings&,
		ve::rendering::VulkanMinecraftDemoSettings& demo_settings,
		double delta_seconds,
		bool ui_captures_input)
	{
		const ve::input::InputSnapshot input = ve::input::CaptureInputSnapshot(window.GetNativeWindow());
		if (ve::input::IsPressed(input, ve::input::Key::Escape)) window.Close();
		static bool was_f3_down = false;
		const bool f3_down = ve::input::IsPressed(input, ve::input::Key::F3);
		if (f3_down && !was_f3_down) demo_settings.show_controls = !demo_settings.show_controls;
		was_f3_down = f3_down;

		if (model.PumpAsyncWorldGeneration() > 0) vulkan_demo_world_controller_.Invalidate();
		vulkan_demo_world_controller_.Update(model, demo_settings.scene, demo_settings.request_scene_rebuild);
		// VulkanFrameRenderer builds its own world mesh; the legacy chunk mesh pipeline uploads OpenGL buffers.
		demo_settings.request_scene_rebuild = false;

		window.SetCursorMode(demo_settings.show_controls ? Window::CursorMode::Normal : Window::CursorMode::Captured);
		const float speed = 18.0f * static_cast<float>(std::max(delta_seconds, 0.001));
		const ve::gameplay::PlayerMoveIntent intent = ve::gameplay::ReadPlayerMoveIntent(input);
		if (!ui_captures_input)
		{
			ve::gameplay::ApplyPlanarMovement(intent, model.MutableCamera(), speed);
			ve::gameplay::ApplyFlyingMovement(intent, model.MutableCamera(), speed);
		}
		if (!ui_captures_input && !demo_settings.show_controls &&
			ve::gameplay::ConsumeBlockBreak(input, input_state_.was_left_mouse_button_pressed))
		{
			const std::optional<ve::gameplay::BlockRaycastHit> hit =
				ve::gameplay::RaycastBlocks(model.GetWorld(), block_registry, model.MutableCamera().GetPosition(), model.MutableCamera().GetForward(), 18.0f);
			if (hit)
			{
				const ve::blocks::BlockId target = model.GetWorld().GetBlock(hit->targetBlock);
				if (!IsTunnelMaterial(target) || !CarveRoundedHole(model.MutableWorld(), block_registry, hit->targetBlock))
				{
					ve::gameplay::BreakBlock(model.MutableWorld(), hit->targetBlock);
				}
			}
		}
	}
}
