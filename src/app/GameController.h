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
		void UpdateFrameGameplay(Window& window,
			ve::world::World& world,
			const ve::blocks::BlockRegistry& block_registry,
			Camera& camera,
			ve::gameplay::BlockSelection& selection,
			ve::gameplay::RuntimeSettings& settings,
			double delta_seconds);
		void ProcessInput(Window& window,
			const ve::world::World& world,
			const ve::blocks::BlockRegistry& block_registry,
			Camera& camera,
			ve::gameplay::RuntimeSettings& settings,
			double delta_seconds);
		void UpdatePlayerMovement(GLFWwindow* window,
			const ve::world::World& world,
			const ve::blocks::BlockRegistry& block_registry,
			Camera& camera,
			ve::gameplay::RuntimeSettings& settings,
			double delta_seconds);
		void ApplyPlayerPhysics(const ve::world::World& world,
			const ve::blocks::BlockRegistry& block_registry,
			Camera& camera,
			ve::gameplay::RuntimeSettings& settings,
			double delta_seconds);
		void UpdateSelection(const ve::world::World& world,
			const ve::blocks::BlockRegistry& block_registry,
			Camera& camera,
			ve::gameplay::BlockSelection& selection);
		void ProcessGameplayInput(Window& window,
			ve::world::World& world,
			const ve::gameplay::BlockSelection& selection,
			ve::gameplay::RuntimeSettings& settings);

		EngineInputState input_state_;
		ve::gameplay::SettingsMenuController settings_menu_controller_;
		VulkanDemoWorldController vulkan_demo_world_controller_;
		ve::blocks::BlockId selected_placement_block_ = ve::blocks::BlockId::Cobblestone;
	};
}
