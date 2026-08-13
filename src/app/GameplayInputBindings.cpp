#include "GameplayInputBindings.h"

#include <array>

namespace ve::gameplay
{
	namespace
	{
		[[nodiscard]] constexpr GameplayInputBinding KeyBinding(
			GameplayAction action,
			ve::input::Key key) noexcept
		{
			return GameplayInputBinding{ action, key };
		}

		[[nodiscard]] constexpr GameplayInputBinding MouseBinding(
			GameplayAction action,
			ve::input::MouseButton button) noexcept
		{
			return GameplayInputBinding{ action, button };
		}

		[[nodiscard]] bool IsControlPressed(
			const ve::input::InputSnapshot& input,
			const GameplayInputControl& control) noexcept
		{
			return std::visit(
				[&input](auto value) { return ve::input::IsPressed(input, value); },
				control);
		}
	}

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

	bool IsGameplayActionPressed(
		const ve::input::InputSnapshot& input,
		GameplayAction action) noexcept
	{
		for (const GameplayInputBinding& binding : DefaultGameplayInputBindings())
		{
			if (binding.action == action && IsControlPressed(input, binding.control)) return true;
		}
		return false;
	}
}
