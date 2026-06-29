	Light Light::Sun(Vec3 direction, float intensity) noexcept
	{
		Light light{};
		light.kind = LightKind::Directional;
		light.direction = direction;
		light.intensity = std::max(0.0f, intensity);
		return light;
	}

	Light Light::Point(Vec3 position, Color color, float intensity, float range) noexcept
	{
		Light light{};
		light.kind = LightKind::Point;
		light.position = position;
		light.color = ClampColor(color);
		light.intensity = std::max(0.0f, intensity);
		light.range = std::max(0.0f, range);
		return light;
	}

	Environment& Environment::Ambient(Color value) noexcept
	{
		ambient = ClampColor(value);
		return *this;
	}

	Environment& Environment::Fog(Color value, float density) noexcept
	{
		fog = ClampColor(value);
		fog_density = std::max(0.0f, density);
		return *this;
	}

	Environment& Environment::TimeOfDay(float hour) noexcept
	{
		time_of_day = std::clamp(hour, 0.0f, 24.0f);
		return *this;
	}
