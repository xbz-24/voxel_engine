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
	void GameController::UpdateVulkanDemo(Window& window, GameModel& model, double delta_seconds)
	{
		if (ve::input::IsPressed(window.GetNativeWindow(), ve::input::Key::Escape)) window.Close();
		model.PumpAsyncWorldGeneration();
		model.TryBuildVulkanDemoScene();
		const float speed = 18.0f * static_cast<float>(std::max(delta_seconds, 0.001));
		const ve::gameplay::PlayerMoveIntent intent = ve::gameplay::ReadPlayerMoveIntent(window.GetNativeWindow());
		ve::gameplay::ApplyPlanarMovement(intent, model.MutableCamera(), speed);
		ve::gameplay::ApplyFlyingMovement(intent, model.MutableCamera(), speed);
	}
}
