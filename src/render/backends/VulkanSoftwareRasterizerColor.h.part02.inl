		const float t = std::clamp(amount, 0.0f, 1.0f);
		const auto channel = [t](std::uint8_t value, std::uint8_t tint_value)
		{
			const float tinted = (ColorChannelFloat(value) * ColorChannelFloat(tint_value)) / 255.0f;
			return ColorChannelByte((ColorChannelFloat(value) * (1.0f - t)) + (tinted * t));
		};
		return { channel(color.r, tint.r), channel(color.g, tint.g), channel(color.b, tint.b) };
	}

	[[nodiscard]] inline Rgb Mix(Rgb left, Rgb right, float amount) noexcept
	{
		const float t = std::clamp(amount, 0.0f, 1.0f);
		const auto lerp = [t](std::uint8_t a, std::uint8_t b)
		{
			return ColorChannelByte((ColorChannelFloat(a) * (1.0f - t)) + (ColorChannelFloat(b) * t));
		};
		return { lerp(left.r, right.r), lerp(left.g, right.g), lerp(left.b, right.b) };
	}

	[[nodiscard]] inline Rgb SkyColor(float direction_y) noexcept
	{
		const float t = std::clamp((direction_y + 0.20f) * 0.80f, 0.0f, 1.0f);
		return Mix({ 78, 107, 150 }, { 166, 210, 255 }, t);
	}
}
