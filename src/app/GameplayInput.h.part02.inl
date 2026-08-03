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
