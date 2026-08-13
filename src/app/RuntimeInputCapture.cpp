#include "RuntimeInput.h"

#include "Input.h"
#include "Window.h"

namespace ve::engine
{
	RuntimeInputSnapshot CaptureRuntimeInputSnapshot(const Window& window) noexcept
	{
		const ve::input::InputSnapshot input = ve::input::CaptureInputSnapshot(window);
		RuntimeInputSnapshot snapshot{};
		snapshot.move_forward = ve::input::IsPressed(input, ve::input::Key::W);
		snapshot.move_left = ve::input::IsPressed(input, ve::input::Key::A);
		snapshot.move_back = ve::input::IsPressed(input, ve::input::Key::S);
		snapshot.move_right = ve::input::IsPressed(input, ve::input::Key::D);
		snapshot.jump = ve::input::IsPressed(input, ve::input::Key::Space);
		snapshot.escape = ve::input::IsPressed(input, ve::input::Key::Escape);
		snapshot.f1 = ve::input::IsPressed(input, ve::input::Key::F1);
		snapshot.f2 = ve::input::IsPressed(input, ve::input::Key::F2);
		snapshot.primary_action = ve::input::IsPressed(input, ve::input::MouseButton::Left);

		const Window::CursorPosition cursor_position = window.CurrentCursorPosition();
		snapshot.mouse_x = cursor_position.x;
		snapshot.mouse_y = cursor_position.y;
		return snapshot;
	}
}
