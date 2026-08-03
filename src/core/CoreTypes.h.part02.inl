
	/** @param value Numeric value to convert. @return Value as a 64-bit float. */
	template <typename Value>
	constexpr double ToDouble(Value value) noexcept
	{
		return static_cast<double>(value);
	}

	/** @param value Object to move from. @return Rvalue reference to the same object. */
	template <typename Value>
	constexpr std::remove_reference_t<Value>&& Move(Value&& value) noexcept
	{
		return static_cast<std::remove_reference_t<Value>&&>(value);
	}

	/** @param value Input value. @param minimum Low bound. @param maximum High bound. @return Clamped value. */
	template <typename Value>
	constexpr Value Clamp(Value value, Value minimum, Value maximum) noexcept
	{
		return value < minimum ? minimum : (maximum < value ? maximum : value);
	}
}
