#pragma once

#include "MouseLookInput.h"

#include <optional>

class Camera;
struct GLFWwindow;

namespace ve::engine
{
	class RuntimeInputRouter
	{
	public:
		void BindMouseLook(
			Camera& camera,
			const bool& ui_captures_mouse,
			const ve::gameplay::MouseLookSettings& mouse_look_settings) noexcept;
		void ClearMouseLookBinding() noexcept;

		[[nodiscard]] std::optional<MouseLookDelta> HandleCursorPosition(
			double current_cursor_x,
			double current_cursor_y) noexcept;

		static void CursorPositionCallback(GLFWwindow* window, double current_cursor_x, double current_cursor_y) noexcept;

	private:
		Camera* camera_ = nullptr;
		const bool* ui_captures_mouse_ = nullptr;
		const ve::gameplay::MouseLookSettings* mouse_look_settings_ = nullptr;
		MouseLookState mouse_look_state_{};
	};
}
