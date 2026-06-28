#include "GameController.h"

#include "BlockInteraction.h"
#include "BlockRegistry.h"
#include "BlockRaycaster.h"
#include "GameplayInput.h"
#include "Hotbar.h"
#include "Input.h"
#include "PlayerMovementInput.h"
#include "RenderDistanceSettings.h"

#include <algorithm>
#include <array>
#include <cmath>
#include <optional>

namespace ve::engine
{
	namespace
	{
		[[nodiscard]] bool IsTunnelMaterial(ve::blocks::BlockId block) noexcept
		{
			using enum ve::blocks::BlockId;
			static constexpr std::array<ve::blocks::BlockId, 16> materials{ {
				Grass, Dirt, Stone, Cobblestone, Gravel, MossBlock, MossyCobblestone, Granite, Andesite, Diorite,
				Deepslate, Sand, Sandstone, RedSand, RedSandstone, Terracotta
			} };
			return std::ranges::find(materials, block) != materials.end();
		}

		[[nodiscard]] bool SetBlockIfChanged(ve::world::World& world, const glm::ivec3& position, ve::blocks::BlockId block)
		{
			return world.GetBlock(position) != block && world.SetBlock(position, block);
		}

		[[nodiscard]] bool CarveRoundedHole(ve::world::World& world,
			const ve::blocks::BlockRegistry& block_registry,
			const glm::ivec3& center)
		{
			bool changed = false;
			constexpr int radius = 3;
			constexpr int depth = 6;
			for (int dx = -radius; dx <= radius; ++dx)
			{
				for (int dz = -radius; dz <= radius; ++dz)
				{
					const float horizontal = static_cast<float>((dx * dx) + (dz * dz)) / static_cast<float>(radius * radius);
					if (horizontal > 1.10f) continue;
					for (int dy = 0; dy >= -depth; --dy)
					{
						const float vertical = static_cast<float>(dy * dy) / static_cast<float>(depth * depth);
						if (horizontal + (vertical * 0.72f) > 1.0f) continue;
						const glm::ivec3 position = center + glm::ivec3{ dx, dy, dz };
						const ve::blocks::BlockId block = world.GetBlock(position);
						if (!block_registry.IsSolid(block) || !IsTunnelMaterial(block)) continue;
						changed = SetBlockIfChanged(world, position, ve::blocks::BlockId::Air) || changed;
					}
				}
			}

			for (int dx = -(radius + 1); dx <= radius + 1; ++dx)
			{
				for (int dz = -(radius + 1); dz <= radius + 1; ++dz)
				{
					const int distance_sq = (dx * dx) + (dz * dz);
					if (distance_sq < (radius * radius) || distance_sq > ((radius + 1) * (radius + 1))) continue;
					const glm::ivec3 rim = center + glm::ivec3{ dx, 0, dz };
					const ve::blocks::BlockId block = world.GetBlock(rim);
					if (!IsTunnelMaterial(block)) continue;
					const ve::blocks::BlockId rim_block = ((dx + dz) & 1) == 0 ? ve::blocks::BlockId::MossBlock : ve::blocks::BlockId::MossyCobblestone;
					changed = SetBlockIfChanged(world, rim, rim_block) || changed;
				}
			}
			return changed;
		}

		void ConsumeFlyToggle(const ve::input::InputSnapshot& input, ve::gameplay::RuntimeSettings& settings, bool& was_pressed)
		{
			if (!ve::input::WasPressed(input, ve::input::Key::F, was_pressed)) return;
			settings.isFlying = !settings.isFlying;
			settings.verticalVelocity = 0.0f;
		}

		void ConsumeRenderDistanceAdjustment(const ve::input::InputSnapshot& input,
			ve::gameplay::RuntimeSettings& settings,
			ve::input::Key key,
			int amount,
			bool& was_pressed)
		{
			if (ve::input::WasPressed(input, key, was_pressed))
			{
				settings.renderDistanceChunks = ve::gameplay::ClampRenderDistanceChunks(settings.renderDistanceChunks + amount);
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
		const ve::input::InputSnapshot input = ve::input::CaptureInputSnapshot(window.GetNativeWindow());
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

	void GameController::ProcessInput(GameplayFrameContext& frame)
	{
		const bool was_menu_open = frame.settings.isSettingsMenuOpen;
		settings_menu_controller_.ProcessInput(frame.window, frame.settings);
		if (was_menu_open || frame.settings.isSettingsMenuOpen) return;
		UpdatePlayerMovement(frame);
	}

	void GameController::UpdatePlayerMovement(GameplayFrameContext& frame)
	{
		ConsumeFlyToggle(frame.input, frame.settings, input_state_.was_fly_toggle_pressed);
		ConsumeRenderDistanceAdjustment(frame.input, frame.settings, ve::input::Key::LeftBracket, -1, input_state_.was_render_distance_decrease_pressed);
		ConsumeRenderDistanceAdjustment(frame.input, frame.settings, ve::input::Key::RightBracket, 1, input_state_.was_render_distance_increase_pressed);

		const float speed = 5.0f * static_cast<float>(frame.delta_seconds);
		const ve::gameplay::PlayerMoveIntent intent = ve::gameplay::ReadPlayerMoveIntent(frame.input);
		ve::gameplay::ApplyPlanarMovement(intent, frame.camera, speed);
		if (frame.settings.isFlying)
		{
			ve::gameplay::ApplyFlyingMovement(intent, frame.camera, speed);
		}
		else if (input_state_.is_grounded && ve::gameplay::WantsJump(intent))
		{
			frame.settings.verticalVelocity = 7.0f;
		}
		ApplyPlayerPhysics(frame);
	}

	void GameController::ApplyPlayerPhysics(GameplayFrameContext& frame)
	{
		if (frame.settings.isFlying)
		{
			frame.settings.verticalVelocity = 0.0f;
			input_state_.is_grounded = false;
			return;
		}

		constexpr float gravity_blocks_per_second = 22.0f;
		constexpr float player_eye_height = 1.7f;
		glm::vec3 position = frame.camera.GetPosition();
		frame.settings.verticalVelocity -= gravity_blocks_per_second * static_cast<float>(frame.delta_seconds);
		position.y += frame.settings.verticalVelocity * static_cast<float>(frame.delta_seconds);

		const int foot_block_x = static_cast<int>(std::floor(position.x));
		const int foot_block_y = static_cast<int>(std::floor(position.y - player_eye_height));
		const int foot_block_z = static_cast<int>(std::floor(position.z));
		if (frame.settings.verticalVelocity <= 0.0f && frame.block_registry.IsSolid(frame.world.GetBlock(foot_block_x, foot_block_y, foot_block_z)))
		{
			position.y = static_cast<float>(foot_block_y) + 1.0f + player_eye_height;
			frame.settings.verticalVelocity = 0.0f;
			input_state_.is_grounded = true;
		}
		else
		{
			input_state_.is_grounded = false;
		}

		frame.camera.MoveTo(position);
	}

	void GameController::UpdateSelection(GameplayFrameContext& frame)
	{
		constexpr float max_reach = 8.0f;
		const std::optional<ve::gameplay::BlockRaycastHit> hit =
			ve::gameplay::RaycastBlocks(frame.world, frame.block_registry, frame.camera.GetPosition(), frame.camera.GetForward(), max_reach);
		frame.selection.has_target = hit.has_value();
		if (hit)
		{
			frame.selection.target_block = hit->targetBlock;
			frame.selection.placement_block = hit->placementBlock;
		}
	}

	void GameController::ProcessGameplayInput(GameplayFrameContext& frame)
	{
		if (frame.settings.isSettingsMenuOpen) return;

		const auto& blocks = ve::gameplay::DefaultHotbarBlocks();
		if (const std::optional<std::size_t> selected_slot = ve::gameplay::ReadSelectedHotbarSlot(frame.input))
		{
			selected_placement_block_ = blocks[*selected_slot];
		}

		if (ve::gameplay::ConsumeDebugToggle(frame.input, input_state_.was_debug_toggle_pressed))
		{
			frame.settings.showDebugOverlay = !frame.settings.showDebugOverlay;
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
