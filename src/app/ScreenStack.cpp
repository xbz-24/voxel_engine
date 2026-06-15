#include "ScreenStack.h"

#include "Window.h"

#include <utility>

namespace ve::engine
{
	/// Pushes a screen and calls its enter hook.
	void ScreenStack::Push(std::unique_ptr<EngineScreen> screen, Window& window)
	{
		if (!screen) return;
		screen->OnEnter(window);
		_screens.push_back(std::move(screen));
	}

	/// Removes the active screen and calls its exit hook.
	void ScreenStack::Pop(Window& window)
	{
		if (_screens.empty()) return;
		_screens.back()->OnExit(window);
		_screens.pop_back();
	}

	/// Returns the active screen.
	EngineScreen* ScreenStack::Active() const noexcept
	{
		if (_screens.empty()) return nullptr;
		return _screens.back().get();
	}

	/// Reports whether no screen is active.
	bool ScreenStack::IsEmpty() const noexcept
	{
		return _screens.empty();
	}
}
