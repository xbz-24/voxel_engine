#pragma once

#include <cstddef>
#include <optional>

struct GLFWwindow;

namespace ve::gameplay
{
	/**
	 * Reads the hotbar slot selected by number keys.
	 *
	 * @param window Native window that owns the input state.
	 * @return Selected hotbar slot, or empty when no slot key is pressed.
	 */
	std::optional<std::size_t> ReadSelectedHotbarSlot(GLFWwindow* window) noexcept;

	/**
	 * Consumes the debug overlay toggle key.
	 *
	 * @param window Native window that owns the input state.
	 * @param wasPressed Previous frame state updated by this call.
	 * @return True only on the first pressed frame.
	 */
	bool ConsumeDebugToggle(GLFWwindow* window, bool& wasPressed) noexcept;

	/**
	 * Consumes the block break mouse action.
	 *
	 * @param window Native window that owns the input state.
	 * @param wasPressed Previous frame state updated by this call.
	 * @return True only on the first pressed frame.
	 */
	bool ConsumeBlockBreak(GLFWwindow* window, bool& wasPressed) noexcept;

	/**
	 * Consumes the block place mouse action.
	 *
	 * @param window Native window that owns the input state.
	 * @param wasPressed Previous frame state updated by this call.
	 * @return True only on the first pressed frame.
	 */
	bool ConsumeBlockPlace(GLFWwindow* window, bool& wasPressed) noexcept;
}
