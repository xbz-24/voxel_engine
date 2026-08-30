#include "VulkanImGuiOverlay.h"
#include "VulkanImGuiOverlayDetail.h"

#include "CoreTypes.h"
#include "Logger.h"
#include "VulkanBackend.h"
#include "Window.h"

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_vulkan.h>

#include <algorithm>
#include <cstdint>
#include <string>

namespace ve::rendering
{
	void VulkanImGuiOverlay::BeginFrame(VulkanOverlaySettings& settings, const VulkanRendererStats& stats)
	{
		has_draw_data_ = false;
		if (!initialized_) return;
		ImGui_ImplVulkan_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		if (settings.show_window)
		{
			ImGui::SetNextWindowSize(ImVec2{ 420.0f, 360.0f }, ImGuiCond_FirstUseEver);
			if (ImGui::Begin("Vulkan Renderer", &settings.show_window))
			{
				if (settings.show_metrics) detail::DrawStats(stats);
				ImGui::Checkbox("Metrics", &settings.show_metrics);
			}
			ImGui::End();
		}
		ImGui::Render();
		const ImDrawData* draw_data = ImGui::GetDrawData();
		has_draw_data_ = draw_data != nullptr && draw_data->TotalVtxCount > 0;
	}
}
