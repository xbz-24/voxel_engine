#include "GameplayInput.h"

#include "Hotbar.h"

#include <array>

namespace ve::gameplay
{
	namespace
	{
		const std::array<ve::input::Key, HotbarSlotCount>& HotbarKeys()
		{
			static constexpr std::array<ve::input::Key, HotbarSlotCount> keys{ {
				ve::input::Key::Digit1,
				ve::input::Key::Digit2,
				ve::input::Key::Digit3,
				ve::input::Key::Digit4,
				ve::input::Key::Digit5,
				ve::input::Key::Digit6,
				ve::input::Key::Digit7,
				ve::input::Key::Digit8,
				ve::input::Key::Digit9
			} };
			return keys;
		}
	}

	bool ConsumeGameplayAction(
		const ve::input::InputSnapshot& input,
		GameplayAction action,
		bool& was_pressed) noexcept
	{
		const bool is_pressed = IsGameplayActionPressed(input, action);
		const bool was_just_pressed = is_pressed && !was_pressed;
		was_pressed = is_pressed;
		return was_just_pressed;
	}

	std::optional<std::size_t> ReadSelectedHotbarSlot(
		const ve::input::InputSnapshot& input) noexcept
	{
		const auto& keys = HotbarKeys();
		for (std::size_t index = 0; index < keys.size(); ++index)
		{
			if (ve::input::IsPressed(input, keys[index])) return index;
		}
		return std::nullopt;
	}

	bool ConsumeDebugToggle(const ve::input::InputSnapshot& input, bool& was_pressed) noexcept
	{
		return ConsumeGameplayAction(input, GameplayAction::ToggleDebugOverlay, was_pressed);
	}

	bool ConsumeBlockBreak(const ve::input::InputSnapshot& input, bool& was_pressed) noexcept
	{
		return ConsumeGameplayAction(input, GameplayAction::BreakBlock, was_pressed);
	}

	bool ConsumeBlockPlace(const ve::input::InputSnapshot& input, bool& was_pressed) noexcept
	{
		return ConsumeGameplayAction(input, GameplayAction::PlaceBlock, was_pressed);
	}
}
