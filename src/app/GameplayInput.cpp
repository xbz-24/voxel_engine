#include "GameplayInput.h"

#include "Hotbar.h"

#include <array>

namespace
{
	using ve::gameplay::GameplayAction;
	using ve::gameplay::GameplayInputBinding;

	[[nodiscard]] constexpr GameplayInputBinding KeyBinding(GameplayAction action, ve::input::Key key) noexcept
	{
		return GameplayInputBinding{ action, key };
	}

	[[nodiscard]] constexpr GameplayInputBinding MouseBinding(GameplayAction action, ve::input::MouseButton button) noexcept
	{
		return GameplayInputBinding{ action, button };
	}

	[[nodiscard]] bool IsControlPressed(
		const ve::input::InputSnapshot& input,
		const ve::gameplay::GameplayInputControl& control) noexcept
	{
		return std::visit(
			[&input](auto value) {
				return ve::input::IsPressed(input, value);
			},
			control);
	}

	[[nodiscard]] bool ConsumeRisingEdge(bool is_pressed, bool& was_pressed) noexcept
	{
		const bool was_just_pressed = is_pressed && !was_pressed;
		was_pressed = is_pressed;
		return was_just_pressed;
	}

	/// Returns the key assigned to each creative hotbar slot.
	const std::array<ve::input::Key, ve::gameplay::HotbarSlotCount>& HotbarKeys()
	{
		static constexpr std::array<ve::input::Key, ve::gameplay::HotbarSlotCount> keys = { {
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

namespace ve::gameplay
{
	std::span<const GameplayInputBinding> DefaultGameplayInputBindings() noexcept
	{
		static constexpr std::array bindings{
			KeyBinding(GameplayAction::MoveForward, ve::input::Key::W),
			KeyBinding(GameplayAction::MoveBackward, ve::input::Key::S),
			KeyBinding(GameplayAction::StrafeLeft, ve::input::Key::A),
			KeyBinding(GameplayAction::StrafeRight, ve::input::Key::D),
			KeyBinding(GameplayAction::Jump, ve::input::Key::Space),
			KeyBinding(GameplayAction::Descend, ve::input::Key::LeftShift),
			KeyBinding(GameplayAction::ToggleFly, ve::input::Key::F),
			KeyBinding(GameplayAction::RenderDistanceDecrease, ve::input::Key::LeftBracket),
			KeyBinding(GameplayAction::RenderDistanceIncrease, ve::input::Key::RightBracket),
			KeyBinding(GameplayAction::ToggleDebugOverlay, ve::input::Key::F3),
			MouseBinding(GameplayAction::BreakBlock, ve::input::MouseButton::Left),
			MouseBinding(GameplayAction::PlaceBlock, ve::input::MouseButton::Right)
		};
		return bindings;
	}

	bool IsGameplayActionPressed(const ve::input::InputSnapshot& input, GameplayAction action) noexcept
	{
		for (const GameplayInputBinding& binding : DefaultGameplayInputBindings())
		{
			if (binding.action == action && IsControlPressed(input, binding.control))
			{
				return true;
			}
		}
		return false;
	}

	bool ConsumeGameplayAction(
		const ve::input::InputSnapshot& input,
		GameplayAction action,
		bool& wasPressed) noexcept
	{
		return ConsumeRisingEdge(IsGameplayActionPressed(input, action), wasPressed);
	}

	/// Reads the hotbar slot selected by number keys.
	std::optional<std::size_t> ReadSelectedHotbarSlot(const ve::input::InputSnapshot& input) noexcept
	{
		const auto& keys = HotbarKeys();
		for (std::size_t index = 0; index < keys.size(); index++)
		{
			if (ve::input::IsPressed(input, keys[index]))
			{
				return index;
			}
		}
		return std::nullopt;
	}

	/// Consumes the debug overlay toggle key.
	bool ConsumeDebugToggle(const ve::input::InputSnapshot& input, bool& wasPressed) noexcept
	{
		return ConsumeGameplayAction(input, GameplayAction::ToggleDebugOverlay, wasPressed);
	}

	/// Consumes the block break mouse action.
	bool ConsumeBlockBreak(const ve::input::InputSnapshot& input, bool& wasPressed) noexcept
	{
		return ConsumeGameplayAction(input, GameplayAction::BreakBlock, wasPressed);
	}

	/// Consumes the block place mouse action.
	bool ConsumeBlockPlace(const ve::input::InputSnapshot& input, bool& wasPressed) noexcept
	{
		return ConsumeGameplayAction(input, GameplayAction::PlaceBlock, wasPressed);
	}
}
