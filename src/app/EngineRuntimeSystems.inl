	/** Creates model, callbacks, editor UI, and view resources. */
	bool EngineRuntime::CreateRuntimeSystems()
	{
		if (!CreateRenderBackend()) return false;
		ve::rendering::VulkanBackend* vulkan_backend = nullptr;
		if (window_.GraphicsApi() == ve::rendering::GraphicsApi::Vulkan)
		{
			vulkan_backend = static_cast<ve::rendering::VulkanBackend*>(backend_.get());
		}
		view_ = RenderViewFactory::Create({ window_.GraphicsApi(), &asset_paths_, vulkan_backend });
		if (view_ == nullptr)
		{
			VE_LOG_CATEGORY_ERROR(ve::log::category::Engine, "Render view creation failed");
			return false;
		}
		const int world_size_chunks = engine_.create_info_.world_size_chunks;
		const auto texture_loading = window_.GraphicsApi() == ve::rendering::GraphicsApi::Vulkan
			? ve::blocks::BlockRegistry::TextureLoading::MetadataOnly
			: ve::blocks::BlockRegistry::TextureLoading::LoadTextures;
		model_ = std::make_unique<GameModel>(world_size_chunks, &asset_paths_, texture_loading);
		vulkan_demo_settings_.scene.preset = engine_.create_info_.vulkan_demo_preset;
		vulkan_demo_settings_.request_scene_rebuild = true;
		callback_context_.camera = &model_->MutableCamera();
		engine_.ConfigureCallbacks(window_, callback_context_);
		if (engine_.create_info_.has_custom_camera)
		{
			model_->MutableCamera().MoveTo(engine_.create_info_.camera_position);
			model_->MutableCamera().TurnTo(engine_.create_info_.camera_look_at);
		}
		else if (window_.GraphicsApi() == ve::rendering::GraphicsApi::Vulkan)
		{
			model_->MutableCamera().MoveTo(glm::vec3(90.0f, 58.0f, 124.0f));
			model_->MutableCamera().TurnTo(glm::vec3(78.0f, 53.0f, 91.0f));
			VE_LOG_CATEGORY_INFO(ve::log::category::Engine, "Vulkan runtime is using the voxel world model");
		}
		else if (window_.GraphicsApi() == ve::rendering::GraphicsApi::OpenGLCompatibility)
		{
			editor_controller_.Initialize(window_, engine_._runtimeSettings);
		}
		return true;
	}

	/** Creates and initializes the selected backend before constructing its view. */
	bool EngineRuntime::CreateRenderBackend()
	{
		backend_ = ve::rendering::RenderBackendFactory::Create(window_.GraphicsApi());
		if (backend_ == nullptr) return false;
		if (window_.GraphicsApi() == ve::rendering::GraphicsApi::OpenGLCompatibility) return true;
		if (window_.GraphicsApi() != ve::rendering::GraphicsApi::Vulkan)
		{
			VE_LOG_CATEGORY_ERROR(ve::log::category::Engine, "Selected render backend is not implemented");
			return false;
		}

		auto& vulkan_backend = static_cast<ve::rendering::VulkanBackend&>(*backend_);
		ve::rendering::VulkanBackendSettings settings{};
#if !defined(NDEBUG)
		if (EnvironmentFlagEnabled("VE_VULKAN_VALIDATION"))
		{
			settings.context.enable_validation_layers = true;
			settings.context.enable_debug_utils = true;
		}
#endif
		if (!vulkan_backend.Initialize(settings, window_))
		{
			VE_LOG_CATEGORY_ERROR(ve::log::category::Engine, "Vulkan backend initialization failed");
			return false;
		}
		if (!vulkan_frame_renderer_.Initialize(vulkan_backend, window_, asset_paths_.blockTexturesDirectory))
		{
			VE_LOG_CATEGORY_ERROR(ve::log::category::Engine, "Vulkan frame renderer initialization failed");
			return false;
		}
		return true;
	}
