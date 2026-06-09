#pragma once

namespace ve::time
{
	class FrameTimer;
}

class Window;

namespace ve::engine
{
	class EngineScreen
	{
	public:
		virtual ~EngineScreen() = default;

		/**
		 * Called when the screen becomes active.
		 *
		 * @param window Window owned by the engine runtime.
		 */
		virtual void OnEnter(Window& window) {}

		/**
		 * Called before the screen is removed or replaced.
		 *
		 * @param window Window owned by the engine runtime.
		 */
		virtual void OnExit(Window& window) {}

		/**
		 * Updates screen logic.
		 *
		 * @param window Window used for input and close requests.
		 * @param frameTimer Frame timing data for the current update.
		 */
		virtual void Update(Window& window, const ve::time::FrameTimer& frameTimer) = 0;

		/**
		 * Draws screen contents.
		 *
		 * @param window Window used for framebuffer dimensions.
		 */
		virtual void Draw(Window& window) = 0;
	};
}
