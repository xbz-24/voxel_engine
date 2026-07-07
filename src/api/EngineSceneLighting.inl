	Light Light::Sun(Vec3 direction, float intensity) noexcept
	{
		Light light{};
		light.kind = LightKind::Directional;
		light.direction = direction;
		light.intensity = std::max(0.0f, intensity);
		light.intensity_unit = LightIntensityUnit::Lux;
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
		light.intensity_unit = LightIntensityUnit::Lumens;
		return light;
	}

	Light Light::Spot(Vec3 position,
		Vec3 direction,
		Color color,
		float intensity,
		float range,
		float inner_cone_degrees,
		float outer_cone_degrees) noexcept
	{
		Light light = Light::Point(position, color, intensity, range);
		light.kind = LightKind::Spot;
		light.direction = direction;
		light.intensity_unit = LightIntensityUnit::Lumens;
		light.SpotCone(inner_cone_degrees, outer_cone_degrees);
		return light;
	}

	Light& Light::UseIntensityUnit(LightIntensityUnit unit) noexcept
	{
		intensity_unit = unit;
		return *this;
	}

	Light& Light::CastShadows(bool enabled, float max_distance, float depth_bias) noexcept
	{
		shadows.enabled = enabled;
		shadows.max_distance = std::max(0.0f, max_distance);
		shadows.depth_bias = std::max(0.0f, depth_bias);
		return *this;
	}

	Light& Light::SpotCone(float inner_degrees, float outer_degrees) noexcept
	{
		inner_cone_degrees = std::clamp(inner_degrees, 0.0f, 90.0f);
		outer_cone_degrees = std::clamp(outer_degrees, inner_cone_degrees, 90.0f);
		return *this;
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
