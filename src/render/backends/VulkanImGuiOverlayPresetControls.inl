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
			settings.scene = VulkanMinecraftDemoDefaultSceneConfig(preset);
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
