TEST_CASE("render backend selector uses the explicitly requested compatibility backend")
{
	const ve::rendering::RenderBackendConfiguration configuration{
		.preferred_api = ve::rendering::GraphicsApi::OpenGLCompatibility
	};

	const ve::rendering::GraphicsApi selected_api = ve::rendering::RenderBackendSelector::SelectApi(configuration);

	CHECK(selected_api == ve::rendering::GraphicsApi::OpenGLCompatibility);
}

TEST_CASE("render backend selector defaults to vulkan when legacy rendering is gone")
{
	const ve::rendering::RenderBackendConfiguration configuration{};

	const ve::rendering::GraphicsApi selected_api = ve::rendering::RenderBackendSelector::SelectApi(configuration);

	CHECK(selected_api == ve::rendering::GraphicsApi::Vulkan);
	CHECK(!configuration.allow_opengl_compatibility_fallback);
}

TEST_CASE("render backend selector supports best available and headless policies")
{
	const ve::rendering::RenderBackendConfiguration best_available{
		.preferred_api = ve::rendering::GraphicsApi::DirectX12,
		.selection_policy = ve::rendering::RenderBackendSelectionPolicy::BestAvailable
	};
	const ve::rendering::RenderBackendSelection best_available_selection =
		ve::rendering::RenderBackendSelector::Select(best_available);

	CHECK(best_available_selection.api == ve::rendering::GraphicsApi::Vulkan);
	CHECK(!best_available_selection.headless);

	const ve::rendering::RenderBackendConfiguration headless{
		.preferred_api = ve::rendering::GraphicsApi::DirectX12,
		.selection_policy = ve::rendering::RenderBackendSelectionPolicy::Headless
	};
	const ve::rendering::RenderBackendSelection headless_selection =
		ve::rendering::RenderBackendSelector::Select(headless);

	CHECK(headless_selection.api == ve::rendering::GraphicsApi::DirectX12);
	CHECK(headless_selection.headless);
}

TEST_CASE("render backend catalog exposes vulkan as the default api")
{
	const ve::rendering::RenderBackendDescriptor& backend = ve::rendering::RenderBackendCatalog::DefaultBackend();

	CHECK(backend.api == ve::rendering::GraphicsApi::Vulkan);
	CHECK(backend.is_default);
	CHECK(backend.is_implemented);
	CHECK(backend.migration.has_backend_neutral_world_renderer);
	CHECK(!backend.migration.has_backend_neutral_hud);
}

TEST_CASE("render backend factory creates every declared backend object")
{
	for (const ve::rendering::RenderBackendDescriptor& descriptor : ve::rendering::RenderBackendCatalog::Backends())
	{
		const std::unique_ptr<ve::rendering::RenderBackend> backend = ve::rendering::RenderBackendFactory::Create(descriptor.api);
		REQUIRE(backend != nullptr);
		CHECK(backend->Api() == descriptor.api);
		CHECK(backend->MigrationStatus().has_runtime_smoke_test == descriptor.migration.has_runtime_smoke_test);
	}
}
