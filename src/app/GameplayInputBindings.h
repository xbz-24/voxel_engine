#pragma once

#include "Input.h"

#include <span>
#include <variant>

namespace ve::gameplay
{
	enum class GameplayAction
	{
		MoveForward,
		MoveBackward,
		StrafeLeft,
		StrafeRight,
		Jump,
		Descend,
		ToggleFly,
		RenderDistanceDecrease,
		RenderDistanceIncrease,
		ToggleDebugOverlay,
		BreakBlock,
		PlaceBlock
	};

	using GameplayInputControl = std::variant<ve::input::Key, ve::input::MouseButton>;

	struct GameplayInputBinding
	{
		GameplayAction action;
		GameplayInputControl control;
	};

	[[nodiscard]] std::span<const GameplayInputBinding> DefaultGameplayInputBindings() noexcept;
	[[nodiscard]] bool IsGameplayActionPressed(
		const ve::input::InputSnapshot& input,
		GameplayAction action) noexcept;
}
