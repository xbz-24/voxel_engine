#include "VulkanImGuiOverlay.h"

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
	namespace
	{
		void CheckVkResult(VkResult result)
		{
			if (result != VK_SUCCESS)
			{
				VE_LOG_CATEGORY_ERROR(ve::log::category::Render,
					"ImGui Vulkan call failed: " + std::to_string(ve::core::ToInt(result)));
			}
		}

		void DrawStats(const VulkanRendererStats& stats)
		{
			ImGui::SeparatorText("Frame");
			ImGui::Text("FPS %d", stats.displayed_fps);
			ImGui::Text("Delta %.2f ms", stats.delta_seconds * 1000.0);
			if (stats.gpu_timing_valid) ImGui::Text("GPU frame %.2f ms", stats.gpu_frame_ms);
			else ImGui::TextUnformatted("GPU frame pending");
			ImGui::Text("Present CPU %.2f ms", stats.present_cpu_ms);
			ImGui::Text("Mesh rebuild CPU %.2f ms", stats.mesh_rebuild_cpu_ms);
			ImGui::Text("Mesh upload CPU %.2f ms", stats.mesh_upload_cpu_ms);
			ImGui::Text("Chunks rebuilt/cached %u/%u", stats.rebuilt_chunk_count, stats.cached_chunk_count);
			ImGui::Text("Indices %u", stats.index_count);
			ImGui::Text("Shadow indices %u", stats.shadow_index_count);
			const std::string world_revision = std::to_string(stats.world_revision);
			ImGui::Text("World revision %s", world_revision.c_str());
			ImGui::Text("Renderer %s", stats.gpu_renderer_active ? "Vulkan GPU" : "CPU fallback");
		}
	}
