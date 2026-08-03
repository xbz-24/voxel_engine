#if !defined(NDEBUG)
		if (EnvironmentFlagEnabled("VE_VULKAN_VALIDATION"))
		{
			settings.context.enable_validation_layers = true;
			settings.context.enable_debug_utils = true;
		}
#endif
		const ve::rendering::VulkanBackendInitializationResult vulkan_backend_result =
			vulkan_backend.InitializeDetailed(settings, window_);
		if (!vulkan_backend_result)
		{
			VE_LOG_CATEGORY_ERROR(ve::log::category::Engine,
				"Vulkan backend initialization failed: " + vulkan_backend_result.message);
			return EngineStartupResult::Failure(
				EngineStartupFailure::RenderBackendInitializationFailed,
				"Vulkan backend initialization failed: " + vulkan_backend_result.message);
		}
		if (!vulkan_frame_orchestrator_.Initialize(vulkan_backend,
			window_,
			asset_paths_.blockTexturesDirectory,
			create_info.voxel_render_style,
			create_info.show_debug_overlay && create_info.settings_menu_enabled))
		{
			VE_LOG_CATEGORY_ERROR(ve::log::category::Engine, "Vulkan frame orchestrator initialization failed");
			return EngineStartupResult::Failure(
				EngineStartupFailure::RenderFrameRendererInitializationFailed,
				"Vulkan frame orchestrator initialization failed");
		}
		return EngineStartupResult::Success();
	}
