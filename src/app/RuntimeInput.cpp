#include "RuntimeInput.h"

#include <array>
#include <optional>

namespace ve::engine
{
	namespace
	{
		constexpr std::array<RuntimeInputAction, 9> TrackedActions{ {
			RuntimeInputAction::MoveForward,
			RuntimeInputAction::MoveLeft,
			RuntimeInputAction::MoveBack,
			RuntimeInputAction::MoveRight,
			RuntimeInputAction::Jump,
			RuntimeInputAction::Cancel,
			RuntimeInputAction::ToggleDebugOverlay,
			RuntimeInputAction::ToggleRenderMode,
			RuntimeInputAction::PrimaryAction
		} };

		[[nodiscard]] std::optional<std::size_t> ActionIndex(RuntimeInputAction action) noexcept
		{
			for (std::size_t index = 0; index < TrackedActions.size(); ++index)
			{
				if (TrackedActions[index] == action) return index;
			}
			return std::nullopt;
		}
	}

	bool IsRuntimeInputActionActive(
		const RuntimeInputSnapshot& input,
		RuntimeInputAction action) noexcept
	{
		switch (action)
		{
		case RuntimeInputAction::MoveForward: return input.move_forward;
		case RuntimeInputAction::MoveLeft: return input.move_left;
		case RuntimeInputAction::MoveBack: return input.move_back;
		case RuntimeInputAction::MoveRight: return input.move_right;
		case RuntimeInputAction::Jump: return input.jump;
		case RuntimeInputAction::Cancel: return input.escape;
		case RuntimeInputAction::ToggleDebugOverlay: return input.f1;
		case RuntimeInputAction::ToggleRenderMode: return input.f2;
		case RuntimeInputAction::PrimaryAction: return input.primary_action;
		default: return false;
		}
	}

	void RuntimeInputActionTracker::Update(const RuntimeInputSnapshot& input) noexcept
	{
		previous_action_states_ = current_action_states_;
		current_action_states_.reset();
		for (std::size_t index = 0; index < TrackedActions.size(); ++index)
		{
			current_action_states_.set(index, IsRuntimeInputActionActive(input, TrackedActions[index]));
		}
	}

	bool RuntimeInputActionTracker::IsDown(RuntimeInputAction action) const noexcept
	{
		const std::optional<std::size_t> index = ActionIndex(action);
		return index.has_value() && current_action_states_[*index];
	}

	bool RuntimeInputActionTracker::WasJustPressed(RuntimeInputAction action) const noexcept
	{
		const std::optional<std::size_t> index = ActionIndex(action);
		return index.has_value() && current_action_states_[*index] && !previous_action_states_[*index];
	}
}
