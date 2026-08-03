		current_action_states_.reset();

		for (std::size_t action_index = 0; action_index < kTrackedRuntimeInputActions.size(); ++action_index)
		{
			const RuntimeInputAction tracked_runtime_input_action =
				kTrackedRuntimeInputActions[action_index];
			current_action_states_.set(
				action_index,
				IsRuntimeInputActionActive(runtime_input_snapshot, tracked_runtime_input_action));
		}
	}

	bool RuntimeInputActionTracker::IsDown(RuntimeInputAction runtime_input_action) const noexcept
	{
		const std::optional<std::size_t> action_index = RuntimeInputActionIndex(runtime_input_action);
		return action_index.has_value() && current_action_states_[*action_index];
	}

	bool RuntimeInputActionTracker::WasJustPressed(RuntimeInputAction runtime_input_action) const noexcept
	{
		const std::optional<std::size_t> action_index = RuntimeInputActionIndex(runtime_input_action);
		return action_index.has_value() &&
			current_action_states_[*action_index] &&
			!previous_action_states_[*action_index];
	}
}
