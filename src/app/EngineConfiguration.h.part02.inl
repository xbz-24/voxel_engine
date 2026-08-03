
	enum class EngineStartupFailure
	{
		None,
		InvalidCreateInfo,
		WindowInitializationFailed,
		RenderBackendUnavailable,
		UnsupportedRenderBackend,
		RenderBackendInitializationFailed,
		RenderFrameRendererInitializationFailed,
		RenderViewCreationFailed
	};

	struct EngineStartupResult
	{
		EngineStartupFailure failure = EngineStartupFailure::None;
		std::string message;

		[[nodiscard]] static EngineStartupResult Success()
		{
			return {};
		}

		[[nodiscard]] static EngineStartupResult Failure(EngineStartupFailure failure, std::string message)
		{
			return EngineStartupResult{ failure, std::move(message) };
		}

		[[nodiscard]] explicit operator bool() const noexcept
		{
			return failure == EngineStartupFailure::None;
		}
	};

	struct EngineCreateInfo
	{
		WindowCreateInfo window{};
		ve::rendering::RenderBackendConfiguration render_backend{};
		RuntimeLogSettings logging{};
		glm::vec3 camera_position{ 80.0f, 50.0f, 80.0f };
		glm::vec3 camera_look_at{ 80.0f, 50.0f, 79.0f };
		bool vsync = false;
		bool show_debug_overlay = true;
		bool settings_menu_enabled = true;
		bool has_custom_camera = false;
		ve::rendering::VulkanMinecraftDemoPreset vulkan_demo_preset = ve::rendering::VulkanMinecraftDemoPreset::HyperrealDesert;
		ve::rendering::VoxelRenderStyle voxel_render_style{};
		int world_size_chunks = 8;
		ve::world::TerrainGenerationSettings terrain_generation{};
		int render_distance_chunks = ve::gameplay::DefaultRenderDistanceChunks;
		std::vector<std::filesystem::path> asset_search_roots;
		std::vector<WorldBlockEdit> world_edits;
		std::function<void(RuntimeFrameContext&)> on_update;
		std::function<void(const RuntimeDiagnostics&)> on_diagnostics;
		std::function<void(const std::string&)> on_log;
	};

	class ValidatedEngineCreateInfo
	{
	public:
		[[nodiscard]] const EngineCreateInfo& Value() const noexcept;

	private:
		friend std::optional<ValidatedEngineCreateInfo> MakeValidatedEngineCreateInfo(
			EngineCreateInfo create_info,
			std::vector<std::string>* issues);

		explicit ValidatedEngineCreateInfo(EngineCreateInfo create_info);

		EngineCreateInfo create_info_;
	};

	[[nodiscard]] std::vector<std::string> ValidateEngineCreateInfo(const EngineCreateInfo& create_info);
	[[nodiscard]] std::optional<ValidatedEngineCreateInfo> MakeValidatedEngineCreateInfo(
		EngineCreateInfo create_info,
		std::vector<std::string>* issues = nullptr);
}
