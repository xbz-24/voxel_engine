		const ve::world::World& world,
		ve::blocks::BlockId selected_placement_block) const;

	/** Draws debug X/Y/Z axes in world space. */
	void renderDebugCoordinateSystemAxes();

	/** Draws the selected block outline in the OpenGL compatibility path. */
	void drawBlockHighlight(glm::ivec3 blockPos, BlockSelectionCube& selectionCube);

	/** Renders the legacy OpenGL 3D scene. */
	void Render3DWorld(ve::engine::RenderView& renderView, const ve::engine::Window& window, Camera& camera, SkyBox& skyBox, Plane& plane, BlockSelectionCube& selectionCube, const ve::blocks::BlockRegistry& blockRegistry, ve::world::World& world, const ve::gameplay::BlockSelection& selection);

	/** Rebuilds 3D and 2D projection matrices after window size changes. */
	void UpdateProjections(int width, int height);

	/** Applies a new framebuffer size when it differs from the cached size. */
	void ApplyFramebufferSize(int width, int height);

	/** @return Validated runtime create info. */
	[[nodiscard]] const ve::engine::EngineCreateInfo& CreateInfo() const noexcept;

	/** @return True when raw create info validation succeeded. */
	[[nodiscard]] bool HasValidCreateInfo() const noexcept;

	/** @return Mutable runtime settings owned by this application facade. */
	[[nodiscard]] ve::gameplay::RuntimeSettings& MutableRuntimeSettings() noexcept;

	/** @return Runtime settings owned by this application facade. */
	[[nodiscard]] const ve::gameplay::RuntimeSettings& RuntimeSettings() const noexcept;

	EngineRenderCacheState _render_cache_state;
	EngineWindowState _window_state;
	ve::gameplay::RuntimeSettings runtime_settings_;
	std::optional<ve::engine::ValidatedEngineCreateInfo> create_info_;
	std::vector<std::string> create_info_validation_issues_;
	std::atomic_bool stop_requested_{ false };
	std::unique_ptr<ve::engine::EngineRuntime> runtime_;
};
