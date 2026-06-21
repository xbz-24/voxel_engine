#include "GameController.h"

#include "BlockInteraction.h"
#include "BlockRaycaster.h"
#include "GameplayInput.h"
#include "Hotbar.h"
#include "Input.h"
#include "PlayerMovementInput.h"

#include <algorithm>
#include <cmath>
#include <optional>

namespace ve::engine
{
	namespace
	{
		void ConsumeFlyToggle(GLFWwindow* window, ve::gameplay::RuntimeSettings& settings, bool& was_pressed)
		{
			if (!ve::input::WasPressed(window, ve::input::Key::F, was_pressed)) return;
			settings.isFlying = !settings.isFlying;
			settings.verticalVelocity = 0.0f;
		}

		void ConsumeRenderDistanceAdjustment(GLFWwindow* window,
			ve::gameplay::RuntimeSettings& settings,
			ve::input::Key key,
			int amount,
			bool& was_pressed)
		{
			if (ve::input::WasPressed(window, key, was_pressed))
			{
				settings.renderDistanceChunks = std::clamp(settings.renderDistanceChunks + amount, 1, 6);
			}
		}
	}

	/// Updates player input, targeting, physics, and block edits.
	void GameController::Update(Window& window,
		GameModel& model,
		const ve::blocks::BlockRegistry& block_registry,
		ve::gameplay::RuntimeSettings& settings,
		double delta_seconds)
	{
		(void)model.PumpAsyncWorldGeneration();
		model.PumpAsyncChunkMeshing(block_registry, settings.renderDistanceChunks);
		UpdateFrameGameplay(window, model.MutableWorld(), block_registry, model.MutableCamera(), model.MutableSelection(), settings, delta_seconds);
	}

	ve::blocks::BlockId GameController::SelectedPlacementBlock() const noexcept
	{
		return selected_placement_block_;
	}

	void GameController::UpdateFrameGameplay(Window& window,
		ve::world::World& world,
		const ve::blocks::BlockRegistry& block_registry,
		Camera& camera,
		ve::gameplay::BlockSelection& selection,
		ve::gameplay::RuntimeSettings& settings,
		double delta_seconds)
	{
		ProcessInput(window, world, block_registry, camera, settings, delta_seconds);
		if (settings.isSettingsMenuOpen)
		{
			selection.has_target = false;
			return;
		}
		UpdateSelection(world, block_registry, camera, selection);
		ProcessGameplayInput(window, world, selection, settings);
		UpdateSelection(world, block_registry, camera, selection);
	}

	void GameController::ProcessInput(Window& window,
		const ve::world::World& world,
		const ve::blocks::BlockRegistry& block_registry,
		Camera& camera,
		ve::gameplay::RuntimeSettings& settings,
		double delta_seconds)
	{
		const bool was_menu_open = settings.isSettingsMenuOpen;
		settings_menu_controller_.ProcessInput(window, settings);
		if (was_menu_open || settings.isSettingsMenuOpen) return;
		UpdatePlayerMovement(window.GetNativeWindow(), world, block_registry, camera, settings, delta_seconds);
	}

	void GameController::UpdatePlayerMovement(GLFWwindow* window,
		const ve::world::World& world,
		const ve::blocks::BlockRegistry& block_registry,
		Camera& camera,
		ve::gameplay::RuntimeSettings& settings,
		double delta_seconds)
	{
		ConsumeFlyToggle(window, settings, input_state_.was_fly_toggle_pressed);
		ConsumeRenderDistanceAdjustment(window, settings, ve::input::Key::LeftBracket, -1, input_state_.was_render_distance_decrease_pressed);
		ConsumeRenderDistanceAdjustment(window, settings, ve::input::Key::RightBracket, 1, input_state_.was_render_distance_increase_pressed);

		const float speed = 5.0f * static_cast<float>(delta_seconds);
		const ve::gameplay::PlayerMoveIntent intent = ve::gameplay::ReadPlayerMoveIntent(window);
		ve::gameplay::ApplyPlanarMovement(intent, camera, speed);
		if (settings.isFlying)
		{
			ve::gameplay::ApplyFlyingMovement(intent, camera, speed);
		}
		else if (input_state_.is_grounded && ve::gameplay::WantsJump(intent))
		{
			settings.verticalVelocity = 7.0f;
		}
		ApplyPlayerPhysics(world, block_registry, camera, settings, delta_seconds);
	}

	void GameController::ApplyPlayerPhysics(const ve::world::World& world,
		const ve::blocks::BlockRegistry& block_registry,
		Camera& camera,
		ve::gameplay::RuntimeSettings& settings,
		double delta_seconds)
	{
		if (settings.isFlying)
		{
			settings.verticalVelocity = 0.0f;
			input_state_.is_grounded = false;
			return;
		}

		constexpr float gravity_blocks_per_second = 22.0f;
		constexpr float player_eye_height = 1.7f;
		glm::vec3 position = camera.GetPosition();
		settings.verticalVelocity -= gravity_blocks_per_second * static_cast<float>(delta_seconds);
		position.y += settings.verticalVelocity * static_cast<float>(delta_seconds);

		const int foot_block_x = static_cast<int>(std::floor(position.x));
		const int foot_block_y = static_cast<int>(std::floor(position.y - player_eye_height));
		const int foot_block_z = static_cast<int>(std::floor(position.z));
		if (settings.verticalVelocity <= 0.0f && block_registry.IsSolid(world.GetBlock(foot_block_x, foot_block_y, foot_block_z)))
		{
			position.y = static_cast<float>(foot_block_y) + 1.0f + player_eye_height;
			settings.verticalVelocity = 0.0f;
			input_state_.is_grounded = true;
		}
		else
		{
			input_state_.is_grounded = false;
		}

		camera.MoveTo(position);
	}

	void GameController::UpdateSelection(const ve::world::World& world,
		const ve::blocks::BlockRegistry& block_registry,
		Camera& camera,
		ve::gameplay::BlockSelection& selection)
	{
		constexpr float max_reach = 8.0f;
		const std::optional<ve::gameplay::BlockRaycastHit> hit =
			ve::gameplay::RaycastBlocks(world, block_registry, camera.GetPosition(), camera.GetForward(), max_reach);
		selection.has_target = hit.has_value();
		if (hit)
		{
			selection.target_block = hit->targetBlock;
			selection.placement_block = hit->placementBlock;
		}
	}

	void GameController::ProcessGameplayInput(Window& window,
		ve::world::World& world,
		const ve::gameplay::BlockSelection& selection,
		ve::gameplay::RuntimeSettings& settings)
	{
		if (settings.isSettingsMenuOpen) return;

		GLFWwindow* native_window = window.GetNativeWindow();
		const auto& blocks = ve::gameplay::DefaultHotbarBlocks();
		if (const std::optional<std::size_t> selected_slot = ve::gameplay::ReadSelectedHotbarSlot(native_window))
		{
			selected_placement_block_ = blocks[*selected_slot];
		}

		if (ve::gameplay::ConsumeDebugToggle(native_window, input_state_.was_debug_toggle_pressed))
		{
			settings.showDebugOverlay = !settings.showDebugOverlay;
		}

		if (selection.has_target && ve::gameplay::ConsumeBlockBreak(native_window, input_state_.was_left_mouse_button_pressed))
		{
			ve::gameplay::BreakBlock(world, selection.target_block);
		}
		if (selection.has_target && ve::gameplay::ConsumeBlockPlace(native_window, input_state_.was_right_mouse_button_pressed))
		{
			ve::gameplay::PlaceBlock(world, selection.placement_block, selected_placement_block_);
		}
	}

	/// Updates the temporary Vulkan migration demo.
	void GameController::UpdateVulkanDemo(Window& window,
		GameModel& model,
		const ve::blocks::BlockRegistry& block_registry,
		ve::gameplay::RuntimeSettings& settings,
		ve::rendering::VulkanMinecraftDemoSettings& demo_settings,
		double delta_seconds,
		bool ui_captures_input)
	{
		if (ve::input::IsPressed(window.GetNativeWindow(), ve::input::Key::Escape)) window.Close();
		static bool was_f3_down = false;
		const bool f3_down = ve::input::IsPressed(window.GetNativeWindow(), ve::input::Key::F3);
		if (f3_down && !was_f3_down) demo_settings.show_controls = !demo_settings.show_controls;
		was_f3_down = f3_down;

		if (model.PumpAsyncWorldGeneration() > 0) vulkan_demo_world_controller_.Invalidate();
		vulkan_demo_world_controller_.Update(model, demo_settings.scene, demo_settings.request_scene_rebuild);
		model.PumpAsyncChunkMeshing(block_registry, settings.renderDistanceChunks);
		demo_settings.request_scene_rebuild = false;

		window.SetCursorMode(demo_settings.show_controls ? Window::CursorMode::Normal : Window::CursorMode::Captured);
		const float speed = 18.0f * static_cast<float>(std::max(delta_seconds, 0.001));
		const ve::gameplay::PlayerMoveIntent intent = ve::gameplay::ReadPlayerMoveIntent(window.GetNativeWindow());
		if (!ui_captures_input)
		{
			ve::gameplay::ApplyPlanarMovement(intent, model.MutableCamera(), speed);
			ve::gameplay::ApplyFlyingMovement(intent, model.MutableCamera(), speed);
		}
	}
}
