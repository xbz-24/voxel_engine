
	/** @param value Numeric value to convert. @return Value as a 64-bit float. */
	template <typename Value>
	constexpr double ToDouble(Value value) noexcept
	{
		return static_cast<double>(value);
	}

}
