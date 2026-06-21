#pragma once

#include <cstddef>
#include <optional>

namespace ve::input
{
	struct InputSnapshot;
}

namespace ve::gameplay
{
	/**
	 * Reads the hotbar slot selected by number keys.
	 *
	 * @param input Captured input state.
	 * @return Selected hotbar slot, or empty when no slot key is pressed.
	 */
	std::optional<std::size_t> ReadSelectedHotbarSlot(const ve::input::InputSnapshot& input) noexcept;

	/**
	 * Consumes the debug overlay toggle key.
	 *
	 * @param input Captured input state.
	 * @param wasPressed Previous frame state updated by this call.
	 * @return True only on the first pressed frame.
	 */
	bool ConsumeDebugToggle(const ve::input::InputSnapshot& input, bool& wasPressed) noexcept;

	/**
	 * Consumes the block break mouse action.
	 *
	 * @param input Captured input state.
	 * @param wasPressed Previous frame state updated by this call.
	 * @return True only on the first pressed frame.
	 */
	bool ConsumeBlockBreak(const ve::input::InputSnapshot& input, bool& wasPressed) noexcept;

	/**
	 * Consumes the block place mouse action.
	 *
	 * @param input Captured input state.
	 * @param wasPressed Previous frame state updated by this call.
	 * @return True only on the first pressed frame.
	 */
	bool ConsumeBlockPlace(const ve::input::InputSnapshot& input, bool& wasPressed) noexcept;
}
