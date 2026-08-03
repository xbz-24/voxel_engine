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
