		void ApplyWindowConfigurationToCreateInfo(ve::engine::EngineCreateInfo& create_info,
			const EngineConfig& config)
		{
			create_info.window.title = config.window.title;
			create_info.window.width = std::max(1, config.window.width);
			create_info.window.height = std::max(1, config.window.height);
			create_info.window.display_index = std::max(0, config.window.monitor_index);
			create_info.window.refresh_rate_hertz = std::max(0, config.window.refresh_rate_hertz);
			create_info.window.fullscreen = config.window.fullscreen;
			create_info.window.resizable = config.window.resizable;
			create_info.window.high_dpi_framebuffer = config.window.high_dpi_framebuffer;
			create_info.window.capture_cursor_on_start = config.window.capture_cursor_on_start;
			create_info.vsync = config.window.vsync;
		}

		void ApplyStartupRenderingConfigurationToCreateInfo(ve::engine::EngineCreateInfo& create_info,
			const EngineConfig& config)
		{
			create_info.render_backend.preferred_api = ToInternalApi(config.graphics_api);
			create_info.vulkan_demo_preset = ToInternalDemoPreset(config.demo_scene);
			create_info.voxel_render_style = ToInternalVoxelRenderStyle(config.voxel_render_style);
			create_info.show_debug_overlay = config.show_debug_overlay;
			create_info.settings_menu_enabled = config.enable_settings_menu;
			create_info.render_distance_chunks = std::max(0, config.render_distance_chunks);
		}

		void ApplyCameraConfigurationToCreateInfo(ve::engine::EngineCreateInfo& create_info,
			const EngineConfig& config)
		{
			create_info.camera_position = ToInternalVec3(config.camera.position);
			create_info.camera_look_at = ToInternalVec3(config.camera.look_at);
			create_info.has_custom_camera = config.camera.has_custom_view;
		}

		void AppendAssetSearchRootsToCreateInfo(ve::engine::EngineCreateInfo& create_info,
			const AssetCatalog& assets)
		{
			create_info.asset_search_roots.reserve(assets.search_roots.size());
			for (const std::string& search_root : assets.search_roots)
			{
				create_info.asset_search_roots.emplace_back(search_root);
			}
		}

		void AppendWorldEditsToCreateInfo(ve::engine::EngineCreateInfo& create_info,
			const WorldConfig& world)
		{
			create_info.world_edits.reserve(world.edits.size());
			for (const WorldEdit& world_edit : world.edits)
			{
				create_info.world_edits.push_back(ToInternalWorldEdit(world_edit));
			}
		}

		void ApplyWorldConfigurationToCreateInfo(ve::engine::EngineCreateInfo& create_info,
			const EngineConfig& config)
		{
			create_info.world_size_chunks = std::max(1, config.world.size_chunks);
			create_info.terrain_generation = ToInternalTerrainGeneration(config.world);
			AppendWorldEditsToCreateInfo(create_info, config.world);
		}
