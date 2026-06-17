#include "GameController.h"

#include "Engine.h"
#include "Input.h"
#include "PlayerMovementInput.h"

#include <algorithm>

namespace ve::engine
{
	/// Updates player input, targeting, physics, and block edits.
	void GameController::Update(Engine& engine, Window& window, GameModel& model, const ve::blocks::BlockRegistry& block_registry, double delta_seconds)
	{
		model.PumpAsyncWorldGeneration();
		model.PumpAsyncChunkMeshing(block_registry, engine._runtimeSettings.renderDistanceChunks);
		engine.UpdateFrameGameplay(window, model.MutableWorld(), block_registry, model.MutableCamera(), model.MutableSelection(), delta_seconds);
	}

	/// Updates the temporary Vulkan migration demo.
	void GameController::UpdateVulkanDemo(Window& window,
		GameModel& model,
		ve::rendering::VulkanMinecraftDemoSettings& demo_settings,
		double delta_seconds,
		bool ui_captures_input)
	{
		if (ve::input::IsPressed(window.GetNativeWindow(), ve::input::Key::Escape)) window.Close();
		static bool was_f3_down = false;
		const bool f3_down = ve::input::IsPressed(window.GetNativeWindow(), ve::input::Key::F3);
		if (f3_down && !was_f3_down) demo_settings.show_controls = !demo_settings.show_controls;
		was_f3_down = f3_down;

		model.PumpAsyncWorldGeneration();
		model.UpdateVulkanDemoScene(demo_settings.scene, demo_settings.request_scene_rebuild);
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
