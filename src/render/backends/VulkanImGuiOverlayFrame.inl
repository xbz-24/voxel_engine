	void VulkanImGuiOverlay::BeginFrame(VulkanMinecraftDemoSettings& settings, const VulkanMinecraftDemoStats& stats)
	{
		has_draw_data_ = false;
		if (!initialized_) return;
		ImGui_ImplVulkan_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		if (settings.show_controls)
		{
			ImGui::SetNextWindowSize(ImVec2{ 420.0f, 620.0f }, ImGuiCond_FirstUseEver);
			if (ImGui::Begin("Vulkan Desert Lab", &settings.show_controls))
			{
				DrawSceneControls(settings);
				if (settings.show_metrics) DrawStats(stats);
				ImGui::SeparatorText("Tools");
				ImGui::Checkbox("Metrics", &settings.show_metrics);
				ImGui::Checkbox("ImGui demo", &settings.show_imgui_demo_window);
			}
			ImGui::End();
		}
		if (settings.show_imgui_demo_window) ImGui::ShowDemoWindow(&settings.show_imgui_demo_window);
		ImGui::Render();
		const ImDrawData* draw_data = ImGui::GetDrawData();
		has_draw_data_ = draw_data != nullptr && draw_data->TotalVtxCount > 0;
	}
