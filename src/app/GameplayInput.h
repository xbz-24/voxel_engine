#pragma once

#include "GameplayInputBindings.h"

#include <cstddef>
#include <optional>

namespace ve::gameplay
{
	/** Consumes an action only on its first pressed frame. */
	[[nodiscard]] bool ConsumeGameplayAction(
		const ve::input::InputSnapshot& input,
		GameplayAction action,
		bool& was_pressed) noexcept;

	/** Returns the hotbar slot selected by the number keys. */
	[[nodiscard]] std::optional<std::size_t> ReadSelectedHotbarSlot(
		const ve::input::InputSnapshot& input) noexcept;

	[[nodiscard]] bool ConsumeDebugToggle(
		const ve::input::InputSnapshot& input,
		bool& was_pressed) noexcept;
	[[nodiscard]] bool ConsumeBlockBreak(
		const ve::input::InputSnapshot& input,
		bool& was_pressed) noexcept;
	[[nodiscard]] bool ConsumeBlockPlace(
		const ve::input::InputSnapshot& input,
		bool& was_pressed) noexcept;
}
