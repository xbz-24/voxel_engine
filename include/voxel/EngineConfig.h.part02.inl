		/** Returns human-readable validation issues without mutating the config. */
		[[nodiscard]] std::vector<std::string> Validate() const;

		/** Returns true when Validate() would produce no issues. */
		[[nodiscard]] bool IsValid() const;

		/** Returns the startup slice consumed before the frame loop exists. */
		[[nodiscard]] EngineStartupConfig StartupConfig() const;

		/** Returns the runtime-tuning slice consumed after startup. */
		[[nodiscard]] EngineRuntimeTuning RuntimeTuning() const noexcept;

		/** Replaces the window creation settings. */
		EngineConfig& WithWindow(WindowConfig value) noexcept;

		/** Replaces the window settings with a windowed title and size. */
		EngineConfig& WithWindow(std::string title, int width = 1280, int height = 720);

		/** Replaces the world generation and edit settings. */
		EngineConfig& WithWorld(WorldConfig value) noexcept;

		/** Alias for WithWorld when the config is used as authored scene content. */
		EngineConfig& WithScene(WorldConfig value) noexcept;

		/** Replaces the asset catalog. */
		EngineConfig& WithAssets(AssetCatalog value) noexcept;

		/** Replaces the material library. */
		EngineConfig& WithMaterials(MaterialLibrary value) noexcept;

		/** Replaces the scene graph request. */
		EngineConfig& WithSceneGraph(SceneGraph value) noexcept;

		/** Replaces the camera startup settings. */
		EngineConfig& WithCamera(CameraConfig value) noexcept;

		/** Sets a custom startup camera by position and look-at target. */
		EngineConfig& WithCamera(Vec3 position, Vec3 look_at) noexcept;

		/** Replaces logging settings. */
		EngineConfig& WithLogging(LogSettings value) noexcept;

		/** Sets the minimum log severity. */
		EngineConfig& WithLogLevel(LogLevel value) noexcept;

		/** Enables file logging to path. */
		EngineConfig& LogToFile(std::string path);

		/** Disables file logging. */
		EngineConfig& DisableFileLogging() noexcept;

		/** Enables or disables console logging. */
		EngineConfig& EnableConsoleLogging(bool enabled = true) noexcept;

		/** Requests a specific graphics backend. */
		EngineConfig& UseGraphicsApi(GraphicsApi value) noexcept;

		/** Requests the Vulkan backend. */
		EngineConfig& UseVulkan() noexcept;

		/** Requests the OpenGL compatibility backend. */
		EngineConfig& UseOpenGLCompatibility() noexcept;

		/** Requests the DirectX 12 backend when available. */
		EngineConfig& UseDirectX12() noexcept;

		/** Replaces voxel lighting, sky, exposure, and fog settings. */
		EngineConfig& WithVoxelRenderStyle(VoxelRenderStyle value) noexcept;

		/** Sets square world size in chunks. */
		EngineConfig& WithWorldSizeChunks(int value) noexcept;

		/** Sets render distance in chunks around the active camera. */
		EngineConfig& WithRenderDistanceChunks(int value) noexcept;

		/** Replaces runtime tuning options. */
		EngineConfig& WithRuntimeTuning(EngineRuntimeTuning value) noexcept;

		/** Enables or disables the debug overlay. */
		EngineConfig& ShowDebugOverlay(bool enabled = true) noexcept;
