namespace ve::rendering
{
	namespace
	{
		void CheckVkResult(VkResult result)
		{
			if (result != VK_SUCCESS)
			{
				VE_LOG_CATEGORY_ERROR(ve::log::category::Render, "ImGui Vulkan call failed: " + std::to_string(static_cast<int>(result)));
			}
		}

		void ApplyPresetDefaults(VulkanMinecraftDemoSettings& settings, VulkanMinecraftDemoPreset preset)
		{
			settings.scene = VulkanMinecraftDemoSceneConfig{};
			settings.scene.preset = preset;
			switch (preset)
			{
			case VulkanMinecraftDemoPreset::AquaModel:
				settings.scene.terrain_radius = 58;
				settings.scene.hill_height = 8;
				settings.scene.tree_count = 0;
				settings.scene.market_stall_count = 0;
				settings.scene.cave = false;
				break;
			case VulkanMinecraftDemoPreset::SponzaAtrium:
				settings.scene.terrain_radius = 72;
				settings.scene.hill_height = 10;
				settings.scene.tree_count = 0;
				settings.scene.market_stall_count = 0;
				settings.scene.cave = false;
				break;
			case VulkanMinecraftDemoPreset::HyperrealDesert:
			default:
				break;
			}
			settings.request_scene_rebuild = true;
		}

		void DrawPresetCombo(VulkanMinecraftDemoSettings& settings)
		{
			static constexpr std::array<VulkanMinecraftDemoPreset, 3> presets{ {
				VulkanMinecraftDemoPreset::HyperrealDesert,
				VulkanMinecraftDemoPreset::AquaModel,
				VulkanMinecraftDemoPreset::SponzaAtrium
			} };
			const char* current = VulkanMinecraftDemoPresetName(settings.scene.preset);
			if (!ImGui::BeginCombo("Scene", current)) return;
			for (const VulkanMinecraftDemoPreset preset : presets)
			{
				const bool selected = preset == settings.scene.preset;
				if (ImGui::Selectable(VulkanMinecraftDemoPresetName(preset), selected))
				{
					ApplyPresetDefaults(settings, preset);
				}
				if (selected) ImGui::SetItemDefaultFocus();
			}
			ImGui::EndCombo();
		}
