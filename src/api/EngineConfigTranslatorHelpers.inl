		[[nodiscard]] ve::engine::WorldBlockEdit ToInternalWorldEdit(const WorldEdit& edit)
		{
			const ve::blocks::BlockId internal_block = ToInternalBlock(edit.block);
			if (edit.kind == WorldEdit::Kind::FillBox)
			{
				return ve::world::MakeBlockBoxEdit(
					edit.box.minimum.x,
					edit.box.minimum.y,
					edit.box.minimum.z,
					edit.box.maximum.x,
					edit.box.maximum.y,
					edit.box.maximum.z,
					internal_block);
			}

			return ve::world::MakeSingleBlockEdit(
				edit.position.x,
				edit.position.y,
				edit.position.z,
				internal_block);
		}

		[[nodiscard]] ve::world::TerrainGeneratorKind ToInternalTerrainGenerator(TerrainGenerator generator) noexcept
		{
			switch (generator)
			{
			case TerrainGenerator::Flat:
				return ve::world::TerrainGeneratorKind::Flat;
			case TerrainGenerator::Procedural:
			default:
				return ve::world::TerrainGeneratorKind::Procedural;
			}
		}

		[[nodiscard]] ve::world::TerrainSpawnBiome ToInternalTerrainBiome(TerrainBiome biome) noexcept
		{
			switch (biome)
			{
			case TerrainBiome::Desert:
				return ve::world::TerrainSpawnBiome::Desert;
			case TerrainBiome::Alpine:
				return ve::world::TerrainSpawnBiome::Alpine;
			case TerrainBiome::Temperate:
			default:
				return ve::world::TerrainSpawnBiome::Temperate;
			}
		}

		[[nodiscard]] ve::world::TerrainBlockPalette ToInternalTerrainPalette(TerrainPalette palette) noexcept
		{
			return ve::world::TerrainBlockPalette{
				ToInternalBlock(palette.bedrock),
				ToInternalBlock(palette.deep_stone),
				ToInternalBlock(palette.stone),
				ToInternalBlock(palette.surface),
				ToInternalBlock(palette.subsurface),
				ToInternalBlock(palette.high_elevation_surface),
				ToInternalBlock(palette.dry_surface_patch),
				ToInternalBlock(palette.gravel_patch)
			};
		}

		[[nodiscard]] ve::world::TerrainGenerationSettings ToInternalTerrainGeneration(const WorldConfig& world) noexcept
		{
			return ve::world::TerrainGenerationSettings{
				world.terrain_seed,
				world.base_surface_height,
				ToInternalTerrainBiome(world.terrain_biome),
				ToInternalTerrainGenerator(world.terrain_generator),
				ToInternalTerrainPalette(world.terrain_palette)
			};
		}

		[[nodiscard]] glm::vec3 ToInternalVec3(Vec3 value) noexcept
		{
			return { value.x, value.y, value.z };
		}

		[[nodiscard]] Vec3 ToPublicVec3(glm::vec3 value) noexcept
		{
			return { value.x, value.y, value.z };
		}

		[[nodiscard]] BlockPosition ToPublicBlockPosition(glm::ivec3 position) noexcept
		{
			return { position.x, position.y, position.z };
		}

		[[nodiscard]] ve::rendering::GraphicsApi ToInternalApi(GraphicsApi api) noexcept
		{
			switch (api)
			{
			case GraphicsApi::OpenGLCompatibility:
				return ve::rendering::GraphicsApi::OpenGLCompatibility;
			case GraphicsApi::DirectX12:
				return ve::rendering::GraphicsApi::DirectX12;
			case GraphicsApi::Vulkan:
			default:
				return ve::rendering::GraphicsApi::Vulkan;
			}
		}

		[[nodiscard]] ve::rendering::VulkanMinecraftDemoPreset ToInternalDemoPreset(DemoScene scene) noexcept
		{
			switch (scene)
			{
			case DemoScene::AquaModel:
				return ve::rendering::VulkanMinecraftDemoPreset::AquaModel;
			case DemoScene::SponzaAtrium:
				return ve::rendering::VulkanMinecraftDemoPreset::SponzaAtrium;
			case DemoScene::HyperrealDesert:
			default:
				return ve::rendering::VulkanMinecraftDemoPreset::HyperrealDesert;
			}
		}

		[[nodiscard]] ve::log::Level ToInternalLogLevel(LogLevel level) noexcept
		{
			switch (level)
			{
			case LogLevel::Trace:
				return ve::log::Level::Trace;
			case LogLevel::Debug:
				return ve::log::Level::Debug;
			case LogLevel::Warning:
				return ve::log::Level::Warning;
			case LogLevel::Error:
				return ve::log::Level::Error;
			case LogLevel::Fatal:
				return ve::log::Level::Fatal;
			case LogLevel::Default:
			case LogLevel::Info:
			default:
				return ve::log::Level::Info;
			}
		}

		[[nodiscard]] ve::engine::RuntimeLogSettings ToInternalLogging(LogSettings logging)
		{
			ve::engine::RuntimeLogSettings result{};
			if (logging.minimum_level != LogLevel::Default)
			{
				result.minimum_level = ToInternalLogLevel(logging.minimum_level);
			}
			result.console_enabled = logging.console_enabled;
			result.file_output_enabled = logging.file_output_enabled;
			result.file_output_path = std::move(logging.file_output_path);
			return result;
		}

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

		[[nodiscard]] FrameContext BuildPublicFrameContextFromRuntimeFrame(
			const ve::engine::RuntimeFrameContext& runtime_frame)
		{
			FrameContext public_frame{};
			public_frame.delta_seconds = runtime_frame.delta_seconds;
			public_frame.elapsed_seconds = runtime_frame.elapsed_seconds;
			public_frame.fps = runtime_frame.fps;
			public_frame.input.move_forward = runtime_frame.input.move_forward;
			public_frame.input.move_left = runtime_frame.input.move_left;
			public_frame.input.move_back = runtime_frame.input.move_back;
			public_frame.input.move_right = runtime_frame.input.move_right;
			public_frame.input.jump = runtime_frame.input.jump;
			public_frame.input.escape = runtime_frame.input.escape;
			public_frame.input.f1 = runtime_frame.input.f1;
			public_frame.input.f2 = runtime_frame.input.f2;
			public_frame.input.primary_action = runtime_frame.input.primary_action;
			public_frame.input.mouse_x = runtime_frame.input.mouse_x;
			public_frame.input.mouse_y = runtime_frame.input.mouse_y;
			public_frame.camera.position = ToPublicVec3(runtime_frame.camera.position);
			public_frame.camera.forward = ToPublicVec3(runtime_frame.camera.forward);
			public_frame.selected_block = ToPublicBlock(runtime_frame.selected_block);
			public_frame.hit_result.has_hit = runtime_frame.hit_result.has_hit;
			public_frame.hit_result.target_block = ToPublicBlockPosition(runtime_frame.hit_result.target_block);
			public_frame.hit_result.placement_block = ToPublicBlockPosition(runtime_frame.hit_result.placement_block);
			public_frame.hit_result.target_block_type = ToPublicBlock(runtime_frame.hit_result.target_block_id);
			return public_frame;
		}

		void AppendPublicFrameCommandsToRuntimeFrame(const FrameContext& public_frame,
			ve::engine::RuntimeFrameContext& runtime_frame)
		{
			runtime_frame.world_edits.reserve(public_frame.commands.world_edits.size());
			for (const WorldEdit& world_edit : public_frame.commands.world_edits)
			{
				runtime_frame.world_edits.push_back(ToInternalWorldEdit(world_edit));
			}
			runtime_frame.request_close = public_frame.commands.request_close;
		}

		[[nodiscard]] Diagnostics BuildPublicDiagnosticsFromRuntimeDiagnostics(
			const ve::engine::RuntimeDiagnostics& runtime_diagnostics)
		{
			return Diagnostics{
				runtime_diagnostics.fps,
				runtime_diagnostics.pending_world_events,
				runtime_diagnostics.render_distance_chunks,
				runtime_diagnostics.pending_chunk_mesh_tasks,
				runtime_diagnostics.pending_chunk_mesh_uploads,
				runtime_diagnostics.pending_world_generation_tasks
			};
		}

		void BindPublicUpdateCallback(ve::engine::EngineCreateInfo& create_info,
			const UpdateCallback& update_callback)
		{
			if (!update_callback)
			{
				return;
			}

			create_info.on_update = [callback = update_callback](ve::engine::RuntimeFrameContext& runtime_frame) {
				FrameContext public_frame = BuildPublicFrameContextFromRuntimeFrame(runtime_frame);
				callback(public_frame);
				AppendPublicFrameCommandsToRuntimeFrame(public_frame, runtime_frame);
			};
		}

		void BindPublicDiagnosticsCallback(ve::engine::EngineCreateInfo& create_info,
			const DiagnosticsCallback& diagnostics_callback)
		{
			if (!diagnostics_callback)
			{
				return;
			}

			create_info.on_diagnostics =
				[callback = diagnostics_callback](const ve::engine::RuntimeDiagnostics& runtime_diagnostics) {
					callback(BuildPublicDiagnosticsFromRuntimeDiagnostics(runtime_diagnostics));
				};
		}
