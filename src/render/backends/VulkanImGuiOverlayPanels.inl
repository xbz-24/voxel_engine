		void DrawSceneControls(VulkanMinecraftDemoSettings& settings)
		{
			DrawPresetCombo(settings);
			bool changed = false;
			changed |= ImGui::DragInt("Seed", &settings.scene.seed, 1.0f, 1, 99999);
			changed |= ImGui::DragInt("Ground", &settings.scene.ground_y, 0.35f, 34, 76);
			changed |= ImGui::DragInt("Dune radius", &settings.scene.terrain_radius, 0.8f, 48, 140);
			changed |= ImGui::DragInt("Dune height", &settings.scene.hill_height, 0.25f, 4, 22);
			changed |= ImGui::DragInt("Rock grains", &settings.scene.house_count, 0.25f, 0, 120);
			changed |= ImGui::DragInt("Dry plants", &settings.scene.tree_count, 1.0f, 0, 520);
			changed |= ImGui::DragInt("Ripple bands", &settings.scene.farm_rows, 0.35f, 0, 48);
			changed |= ImGui::DragInt("Oasis radius", &settings.scene.water_radius, 0.25f, 0, 14);
			changed |= ImGui::DragInt("Ridge height", &settings.scene.tower_height, 0.25f, 2, 14);
			changed |= ImGui::DragInt("Heat glints", &settings.scene.lantern_count, 0.35f, 0, 24);
			changed |= ImGui::DragInt("Strata veins", &settings.scene.ore_richness, 0.35f, 0, 56);
			changed |= ImGui::DragInt("Pebble fields", &settings.scene.market_stall_count, 0.5f, 0, 220);
			changed |= ImGui::DragInt("Ruin fragments", &settings.scene.ruin_count, 0.15f, 0, 8);
			changed |= ImGui::DragInt("Survey markers", &settings.scene.vista_marker_count, 0.15f, 0, 12);
			changed |= ImGui::Checkbox("Oasis", &settings.scene.water);
			ImGui::SameLine();
			changed |= ImGui::Checkbox("Canyons", &settings.scene.cave);
			ImGui::SameLine();
			changed |= ImGui::Checkbox("Plants", &settings.scene.trees);
			changed |= ImGui::Checkbox("Heat glints", &settings.scene.lights);
			ImGui::SameLine();
			changed |= ImGui::Checkbox("Ruins", &settings.scene.ruins);
			ImGui::SameLine();
			changed |= ImGui::Checkbox("Dense grains", &settings.scene.stress_blocks);
			if (changed) settings.request_scene_rebuild = true;

			if (ImGui::Button("Rebuild"))
			{
				settings.request_scene_rebuild = true;
			}
			ImGui::SameLine();
			if (ImGui::Button("New seed"))
			{
				settings.scene.seed = (settings.scene.seed * 1103515245 + 12345) & 0x7FFFFFFF;
				settings.request_scene_rebuild = true;
			}
			ImGui::SameLine();
			if (ImGui::Button("Reset scene"))
			{
				ApplyPresetDefaults(settings, settings.scene.preset);
			}
		}

		void DrawStats(const VulkanMinecraftDemoStats& stats)
		{
			ImGui::SeparatorText("Frame");
			ImGui::Text("FPS %d", stats.displayed_fps);
			ImGui::Text("Delta %.2f ms", stats.delta_seconds * 1000.0);
			if (stats.gpu_timing_valid) ImGui::Text("GPU frame %.2f ms", stats.gpu_frame_ms);
			else ImGui::TextUnformatted("GPU frame pending");
			ImGui::Text("Present CPU %.2f ms", stats.present_cpu_ms);
			ImGui::Text("Mesh rebuild CPU %.2f ms", stats.mesh_rebuild_cpu_ms);
			ImGui::Text("Mesh upload CPU %.2f ms", stats.mesh_upload_cpu_ms);
			ImGui::Text("Indices %u", stats.index_count);
			ImGui::Text("World revision %llu", static_cast<unsigned long long>(stats.world_revision));
			ImGui::Text("Renderer %s", stats.gpu_renderer_active ? "Vulkan GPU" : "CPU fallback");
		}
	}
