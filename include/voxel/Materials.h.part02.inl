	{
		Directional,
		Point,
		Spot
	};

	/** Unit used to interpret Light::intensity. */
	enum class LightIntensityUnit
	{
		Relative,
		Lux,
		Lumens,
		Candela
	};

	/** Optional shadow-casting request for a light. */
	struct ShadowSettings
	{
		bool enabled = false;
		float max_distance = 128.0f;
		float depth_bias = 0.005f;
	};

	/** Public light description for scene graphs. */
	struct Light
	{
		LightKind kind = LightKind::Point;
		LightIntensityUnit intensity_unit = LightIntensityUnit::Relative;
		Vec3 position{};
		Vec3 direction{ 0.0f, -1.0f, 0.0f };
		Color color{};
		float intensity = 1.0f;
		float range = 16.0f;
		float inner_cone_degrees = 20.0f;
		float outer_cone_degrees = 30.0f;
		ShadowSettings shadows{};

		/** Creates a directional light. */
		[[nodiscard]] static Light Sun(Vec3 direction, float intensity = 1.0f) noexcept;

		/** Creates a point light. */
		[[nodiscard]] static Light Point(Vec3 position, Color color = {}, float intensity = 1.0f, float range = 16.0f) noexcept;

		/** Creates a spot light. */
		[[nodiscard]] static Light Spot(Vec3 position,
			Vec3 direction,
			Color color = {},
			float intensity = 1.0f,
			float range = 16.0f,
			float inner_cone_degrees = 20.0f,
			float outer_cone_degrees = 30.0f) noexcept;

		/** Sets the unit used to interpret intensity. */
		Light& UseIntensityUnit(LightIntensityUnit unit) noexcept;

		/** Enables or disables shadow casting with distance and bias controls. */
		Light& CastShadows(bool enabled = true, float max_distance = 128.0f, float depth_bias = 0.005f) noexcept;

		/** Sets inner and outer spot cone angles in degrees. */
		Light& SpotCone(float inner_degrees, float outer_degrees) noexcept;
	};

	/** Scene-wide environmental lighting and fog request. */
	struct Environment
	{
		Color ambient{ 0.35f, 0.38f, 0.44f, 1.0f };
		Color fog{ 0.56f, 0.68f, 0.84f, 1.0f };
		float fog_density = 0.0f;
		float time_of_day = 12.0f;

		/** Sets ambient light color. */
		Environment& Ambient(Color value) noexcept;

		/** Sets fog color and density. */
		Environment& Fog(Color value, float density) noexcept;

		/** Sets authored time of day in hours. */
		Environment& TimeOfDay(float hour) noexcept;
	};

	/** Collection of named materials referenced by authored scene data. */
	struct MaterialLibrary
	{
		std::vector<Material> materials;

		/** Adds or appends a material definition. */
		MaterialLibrary& Add(Material material);

