				const ve::blocks::BlockRegistry& block_registry,
				Camera& camera,
				ve::gameplay::BlockSelection& selection) noexcept
				: world(world),
				  block_registry(block_registry),
				  camera(camera),
				  selection(selection)
			{
			}

			ve::world::World& world;
			const ve::blocks::BlockRegistry& block_registry;
			Camera& camera;
			ve::gameplay::BlockSelection& selection;
		};

		struct GameplayCommandFrameContext
		{
			GameplayCommandFrameContext(const ve::input::InputSnapshot& input,
				ve::world::World& world,
				ve::gameplay::BlockSelection& selection,
				ve::gameplay::RuntimeSettings& settings) noexcept
				: input(input),
				  world(world),
				  selection(selection),
				  settings(settings)
			{
			}

			const ve::input::InputSnapshot& input;
			ve::world::World& world;
			ve::gameplay::BlockSelection& selection;
			ve::gameplay::RuntimeSettings& settings;
		};

		void UpdateFrameGameplay(
			Window& window,
			const ve::input::InputSnapshot& input,
			ve::world::World& world,
			const ve::blocks::BlockRegistry& block_registry,
			Camera& camera,
			ve::gameplay::BlockSelection& selection,
			ve::gameplay::RuntimeSettings& settings,
			double delta_seconds);
		void ProcessInput(Window& window, PlayerMovementFrameContext& movement_frame);
		void UpdatePlayerMovement(PlayerMovementFrameContext& frame);
		void ApplyPlayerPhysics(PlayerMovementFrameContext& frame);
		void UpdateSelection(BlockSelectionFrameContext& frame);
		void ProcessGameplayInput(GameplayCommandFrameContext& frame);

		EngineInputState input_state_;
		ve::gameplay::SettingsMenuController settings_menu_controller_;
		VulkanDemoWorldController vulkan_demo_world_controller_;
		ve::blocks::BlockId selected_placement_block_ = ve::blocks::BlockId::Cobblestone;
	};
}
