		class EngineConfigTranslator final : public IEngineConfigTranslator
		{
		public:
			[[nodiscard]] ve::engine::EngineCreateInfo Translate(const EngineConfig& config) const override
			{
				ve::engine::EngineCreateInfo result{};
				result.window.title = config.window.title;
				result.window.width = std::max(1, config.window.width);
				result.window.height = std::max(1, config.window.height);
				result.window.fullscreen = config.window.fullscreen;
				result.render_backend.preferred_api = ToInternalApi(config.graphics_api);
				result.vulkan_demo_preset = ToInternalDemoPreset(config.demo_scene);
				result.camera_position = ToInternalVec3(config.camera.position);
				result.camera_look_at = ToInternalVec3(config.camera.look_at);
				result.has_custom_camera = config.camera.has_custom_view;
				result.vsync = config.window.vsync;
				result.show_debug_overlay = config.show_debug_overlay;
				result.world_size_chunks = std::max(1, config.world.size_chunks);
				result.render_distance_chunks = std::max(0, config.render_distance_chunks);
				result.world_edits.reserve(config.world.edits.size());
				for (const WorldEdit& edit : config.world.edits)
				{
					result.world_edits.push_back(ToInternalWorldEdit(edit));
				}
				if (config.on_update)
				{
					result.on_update = [callback = config.on_update](ve::engine::RuntimeFrameContext& frame) {
						FrameContext public_frame{};
						public_frame.delta_seconds = frame.delta_seconds;
						public_frame.elapsed_seconds = frame.elapsed_seconds;
						public_frame.fps = frame.fps;
						public_frame.input.move_forward = frame.input.move_forward;
						public_frame.input.move_left = frame.input.move_left;
						public_frame.input.move_back = frame.input.move_back;
						public_frame.input.move_right = frame.input.move_right;
						public_frame.input.jump = frame.input.jump;
						public_frame.input.escape = frame.input.escape;
						public_frame.input.f1 = frame.input.f1;
						public_frame.input.f2 = frame.input.f2;
						public_frame.input.primary_action = frame.input.primary_action;
						public_frame.input.mouse_x = frame.input.mouse_x;
						public_frame.input.mouse_y = frame.input.mouse_y;
						callback(public_frame);
						frame.world_edits.reserve(public_frame.commands.world_edits.size());
						for (const WorldEdit& edit : public_frame.commands.world_edits)
						{
							frame.world_edits.push_back(ToInternalWorldEdit(edit));
						}
						frame.request_close = public_frame.commands.request_close;
					};
				}
				if (config.on_diagnostics)
				{
					result.on_diagnostics = [callback = config.on_diagnostics](const ve::engine::RuntimeDiagnostics& diagnostics) {
						callback(Diagnostics{
							diagnostics.fps,
							diagnostics.pending_world_events,
							diagnostics.render_distance_chunks
						});
					};
				}
				result.on_log = config.on_log;
				return result;
			}
		};
	}
