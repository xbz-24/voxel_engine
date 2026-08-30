#pragma once


#include "Block.h"
#include "EngineState.h"
#include "GameModel.h"
#include "RuntimeSettings.h"
#include "SettingsMenuController.h"

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
	namespace game_controller_detail
	{
		struct BlockSelectionFrameContext;
		struct GameplayCommandFrameContext;
		struct PlayerMovementFrameContext;
	}

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
		 * Updates Vulkan world generation, camera input, and block interaction.
		 *
		 * @param window Runtime window used for input.
		 * @param model Mutable gameplay model.
		 * @param delta_seconds Time elapsed since the previous frame.
		 */
		void UpdateVulkanWorld(Window& window,
			GameModel& model,
			const ve::blocks::BlockRegistry& block_registry,
			ve::gameplay::RuntimeSettings& settings,
			double delta_seconds,
			bool ui_captures_input);

	private:
		void UpdateFrameGameplay(
			Window& window,
			const ve::input::InputSnapshot& input,
			ve::world::World& world,
			const ve::blocks::BlockRegistry& block_registry,
			Camera& camera,
			ve::gameplay::BlockSelection& selection,
			ve::gameplay::RuntimeSettings& settings,
			double delta_seconds);
		void ProcessInput(Window& window, game_controller_detail::PlayerMovementFrameContext& frame);
		void UpdatePlayerMovement(game_controller_detail::PlayerMovementFrameContext& frame);
		void ApplyPlayerPhysics(game_controller_detail::PlayerMovementFrameContext& frame);
		void UpdateSelection(game_controller_detail::BlockSelectionFrameContext& frame);
		void ProcessGameplayInput(game_controller_detail::GameplayCommandFrameContext& frame);

		EngineInputState input_state_;
		ve::gameplay::SettingsMenuController settings_menu_controller_;
		ve::blocks::BlockId selected_placement_block_ = ve::blocks::BlockId::Cobblestone;
	};
}
