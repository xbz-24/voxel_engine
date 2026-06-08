#include "GameplayInput.h"

#include "Hotbar.h"
#include "Input.h"

#include <array>

namespace
{
	/// Returns the key assigned to each creative hotbar slot.
	const std::array<ve::input::Key, ve::gameplay::HotbarSlotCount>& HotbarKeys()
	{
		static constexpr std::array<ve::input::Key, ve::gameplay::HotbarSlotCount> keys = {
			ve::input::Key::Digit1,
			ve::input::Key::Digit2,
			ve::input::Key::Digit3,
			ve::input::Key::Digit4,
			ve::input::Key::Digit5,
			ve::input::Key::Digit6,
			ve::input::Key::Digit7,
			ve::input::Key::Digit8,
			ve::input::Key::Digit9
		};
		return keys;
	}
}

namespace ve::gameplay
{
	/// Reads the hotbar slot selected by number keys.
	std::optional<std::size_t> ReadSelectedHotbarSlot(GLFWwindow* window) noexcept
	{
		const auto& keys = HotbarKeys();
		for (std::size_t index = 0; index < keys.size(); index++)
		{
			if (ve::input::IsPressed(window, keys[index]))
			{
				return index;
			}
		}
		return std::nullopt;
	}

	/// Consumes the debug overlay toggle key.
	bool ConsumeDebugToggle(GLFWwindow* window, bool& wasPressed) noexcept
	{
		return ve::input::WasPressed(window, ve::input::Key::F3, wasPressed);
	}

	/// Consumes the block break mouse action.
	bool ConsumeBlockBreak(GLFWwindow* window, bool& wasPressed) noexcept
	{
		return ve::input::WasPressed(window, ve::input::MouseButton::Left, wasPressed);
	}

	/// Consumes the block place mouse action.
	bool ConsumeBlockPlace(GLFWwindow* window, bool& wasPressed) noexcept
	{
		return ve::input::WasPressed(window, ve::input::MouseButton::Right, wasPressed);
	}
}
