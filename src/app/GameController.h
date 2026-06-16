#pragma once

#include "GameModel.h"

class Engine;
class Window;

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
		 * @param engine Engine services still being migrated out of the old controller.
		 * @param window Runtime window used for input.
		 * @param model Mutable gameplay model.
		 * @param block_registry Block metadata used by collision/raycast logic.
		 * @param delta_seconds Time elapsed since the previous frame.
		 */
		void Update(Engine& engine, Window& window, GameModel& model, const ve::blocks::BlockRegistry& block_registry, double delta_seconds);

		/**
		 * Updates the temporary Vulkan demo camera and async world state.
		 *
		 * @param window Runtime window used for input.
		 * @param model Mutable gameplay model.
		 * @param delta_seconds Time elapsed since the previous frame.
		 */
		void UpdateVulkanDemo(Window& window, GameModel& model, double delta_seconds);
	};
}
