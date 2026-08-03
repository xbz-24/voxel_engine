
		/** Sets the world-space direction toward the sun. */
		VoxelRenderStyle& WithSunDirection(Vec3 direction) noexcept;

		/** Sets the linear sun color; HDR channel values greater than one are allowed. */
		VoxelRenderStyle& WithSunColor(Vec3 color) noexcept;

		/** Sets non-negative direct-sun intensity. */
		VoxelRenderStyle& WithSunIntensity(float intensity) noexcept;

		/** Sets linear horizon and zenith colors used by sky and aerial perspective. */
		VoxelRenderStyle& WithSkyColors(Vec3 horizon, Vec3 zenith) noexcept;

		/** Sets positive tone-mapping exposure. */
		VoxelRenderStyle& WithExposure(float value) noexcept;

		/** Sets the distance interval over which atmospheric fog accumulates. */
		VoxelRenderStyle& WithFogRange(float start_distance, float end_distance) noexcept;

		/** Sets normalized distance-fog strength. */
		VoxelRenderStyle& WithFogStrength(float strength) noexcept;

		/** Sets normalized cloud coverage/density and non-negative animation speed. */
		VoxelRenderStyle& WithClouds(float coverage, float density, float speed) noexcept;

		/** Sets procedural surface-normal detail strength in the 0..2 authoring range. */
		VoxelRenderStyle& WithSurfaceDetail(float strength) noexcept;

		/** Sets water reflection strength in the 0..2 authoring range. */
		VoxelRenderStyle& WithWaterReflections(float strength) noexcept;

		/** Sets procedural contact-shadow strength in the 0..2 authoring range. */
		VoxelRenderStyle& WithShadowStrength(float strength) noexcept;

		/** Sets direct-light specular strength in the 0..2 authoring range. */
		VoxelRenderStyle& WithSpecularStrength(float strength) noexcept;

		/** Sets procedural ambient-occlusion strength in the 0..2 authoring range. */
		VoxelRenderStyle& WithAmbientOcclusion(float strength) noexcept;

		/** Sets atmospheric in-scattering strength in the 0..2 authoring range. */
		VoxelRenderStyle& WithAerialPerspective(float strength) noexcept;

		/** Sets foliage and snow subsurface response in the 0..2 authoring range. */
		VoxelRenderStyle& WithSubsurfaceScattering(float strength) noexcept;

		/** Sets cloud self-shadow absorption in the 0..2 authoring range. */
		VoxelRenderStyle& WithCloudLightAbsorption(float strength) noexcept;

		/** Sets directional shadow intensity, PCF softness, and world-space coverage distance. */
		VoxelRenderStyle& WithDirectionalShadows(
			float strength,
			float softness,
			float distance) noexcept;
	};

	/** Minimum severity routed through the engine logger. */
	enum class LogLevel
	{
		Default,
		Trace,
		Debug,
		Info,
		Warning,
		Error,
		Fatal
	};

	/** Logging policy translated into the runtime logger at startup. */
	struct LogSettings
	{
		LogLevel minimum_level = LogLevel::Default;
		bool console_enabled = true;
		bool file_output_enabled = true;
		std::string file_output_path;

		/** Sets the lowest severity that should be emitted. */
		LogSettings& WithMinimumLevel(LogLevel level) noexcept;

		/** Enables or disables console log output. */
		LogSettings& EnableConsole(bool enabled = true) noexcept;

		/** Disables console log output. */
		LogSettings& DisableConsole() noexcept;

		/** Enables file output and writes logs to path. */
		LogSettings& WriteToFile(std::string path);

