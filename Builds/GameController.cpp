#include "GameController.h"

#include "Engine.h"

namespace ve::engine
{
	/// Updates player input, targeting, physics, and block edits.
	void GameController::Update(Engine& engine, Window& window, GameModel& model, const ve::blocks::BlockRegistry& block_registry, double delta_seconds)
	{
		model.PumpAsyncWorldGeneration();
		engine.UpdateFrameGameplay(window, model.MutableWorld(), block_registry, model.MutableCamera(), model.MutableSelection(), delta_seconds);
	}
}
