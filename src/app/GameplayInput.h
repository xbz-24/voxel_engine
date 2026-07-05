#pragma once

#include "Input.h"

#include <cstddef>
#include <optional>
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

	/**
	 * Returns the default data-driven action bindings used by gameplay.
	 *
	 * @return Immutable action binding table.
	 */
	[[nodiscard]] std::span<const GameplayInputBinding> DefaultGameplayInputBindings() noexcept;

	/**
	 * Reads whether a gameplay action is held down.
	 *
	 * @param input Captured input state.
	 * @param action Gameplay action to query.
	 * @return True when any bound control is pressed.
	 */
	[[nodiscard]] bool IsGameplayActionPressed(
		const ve::input::InputSnapshot& input,
		GameplayAction action) noexcept;

	/**
	 * Consumes a gameplay action as a rising edge.
	 *
	 * @param input Captured input state.
	 * @param action Gameplay action to consume.
	 * @param wasPressed Previous frame state updated by this call.
	 * @return True only on the first pressed frame.
	 */
	[[nodiscard]] bool ConsumeGameplayAction(
		const ve::input::InputSnapshot& input,
		GameplayAction action,
		bool& wasPressed) noexcept;

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
