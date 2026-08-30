#pragma once

namespace voxel
{
	/** @addtogroup voxel_sdk_runtime_callbacks
	 * @{
	 */
	/** Keyboard keys exposed to public frame callbacks. */
	enum class Key
	{
		Unknown,
		W,
		A,
		S,
		D,
		Space,
		Escape,
		F1,
		F2
	};

	/** Semantic input actions mapped from raw runtime input. */
	enum class InputAction
	{
		MoveForward,
		MoveLeft,
		MoveBack,
		MoveRight,
		Jump,
		Cancel,
		ToggleDebugOverlay,
		ToggleRenderMode,
		PrimaryAction
	};

	/** Input state captured for one public update callback frame. */
	struct InputSnapshot
	{
		bool move_forward = false;
		bool move_left = false;
		bool move_back = false;
		bool move_right = false;
		bool jump = false;
		bool escape = false;
		bool f1 = false;
		bool f2 = false;
		bool primary_action = false;
		double mouse_x = 0.0;
		double mouse_y = 0.0;

		[[nodiscard]] bool IsDown(Key key) const noexcept;
		[[nodiscard]] bool IsActive(InputAction action) const noexcept;
	};
	/** @} */
}
