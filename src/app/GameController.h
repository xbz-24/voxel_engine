#pragma once

#include "Block.h"
#include "EngineState.h"
#include "GameModel.h"
#include "RuntimeSettings.h"
#include "SettingsMenuController.h"
#include "VulkanDemoWorldController.h"
#include "VulkanMinecraftDemoSettings.h"

namespace ve::engine { class Window; }

namespace ve::blocks
{
	class BlockRegistry;
}

namespace ve::input
{
	struct InputSnapshot;
}

namespace ve::engine
{
	class GameController
	{
	public:
		/**
		 * Updates player input, targeting, physics, and block edits.
		 *
		 * @param window Runtime window used for input.
		 * @param model Mutable gameplay model.
		 * @param block_registry Block metadata used by collision/raycast logic.
		 * @param settings Runtime options shared by gameplay and HUD.
		 * @param delta_seconds Time elapsed since the previous frame.
		 */
		void Update(Window& window,
			GameModel& model,
			const ve::blocks::BlockRegistry& block_registry,
			ve::gameplay::RuntimeSettings& settings,
			double delta_seconds);

		/** @return Block id selected for placement by hotbar input. */
		[[nodiscard]] ve::blocks::BlockId SelectedPlacementBlock() const noexcept;

		/**
		 * Updates the temporary Vulkan demo camera and async world state.
		 *
		 * @param window Runtime window used for input.
		 * @param model Mutable gameplay model.
		 * @param delta_seconds Time elapsed since the previous frame.
		 */
		void UpdateVulkanDemo(Window& window,
			GameModel& model,
			const ve::blocks::BlockRegistry& block_registry,
			ve::gameplay::RuntimeSettings& settings,
			ve::rendering::VulkanMinecraftDemoSettings& demo_settings,
			double delta_seconds,
			bool ui_captures_input);

	private:
		struct PlayerMovementFrameContext
		{
			PlayerMovementFrameContext(const ve::input::InputSnapshot& input,
				ve::world::World& world,
				const ve::blocks::BlockRegistry& block_registry,
				Camera& camera,
				ve::gameplay::RuntimeSettings& settings,
				double delta_seconds) noexcept
				: input(input),
				  world(world),
				  block_registry(block_registry),
				  camera(camera),
				  settings(settings),
				  delta_seconds(delta_seconds)
			{
			}

			const ve::input::InputSnapshot& input;
			ve::world::World& world;
			const ve::blocks::BlockRegistry& block_registry;
			Camera& camera;
			ve::gameplay::RuntimeSettings& settings;
			double delta_seconds;
		};

		struct BlockSelectionFrameContext
		{
			BlockSelectionFrameContext(ve::world::World& world,
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
		// TODO: Split editor/menu navigation from survival gameplay so non-demo apps can opt out cleanly.
		ve::gameplay::SettingsMenuController settings_menu_controller_;
		VulkanDemoWorldController vulkan_demo_world_controller_;
		ve::blocks::BlockId selected_placement_block_ = ve::blocks::BlockId::Cobblestone;
	};
}
