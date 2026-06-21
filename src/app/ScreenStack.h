#pragma once

#include "EngineScreen.h"

#include <memory>
#include <vector>

namespace ve::engine
{
	class ScreenStack
	{
	public:
		/**
		 * Pushes a screen and calls its enter hook.
		 *
		 * @param screen Screen owned by the stack.
		 * @param window Runtime window passed to lifecycle hooks.
		 */
		void Push(std::unique_ptr<EngineScreen> screen, Window& window);

		/**
		 * Removes the active screen and calls its exit hook.
		 *
		 * @param window Runtime window passed to lifecycle hooks.
		 */
		void Pop(Window& window);

		/**
		 * Returns the active screen.
		 *
		 * @return Active screen, or nullptr when the stack is empty.
		 */
		EngineScreen* Active() const noexcept;

		/**
		 * Reports whether no screen is active.
		 *
		 * @return True when the stack has no screens.
		 */
		bool IsEmpty() const noexcept;

	private:
		std::vector<std::unique_ptr<EngineScreen>> _screens;
	};
}
