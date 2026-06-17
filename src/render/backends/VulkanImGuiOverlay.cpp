#include "VulkanImGuiOverlay.h"

#include "Logger.h"
#include "VulkanBackend.h"
#include "Window.h"

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_vulkan.h>

#include <algorithm>
#include <array>
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
				VE_LOG_CATEGORY_ERROR(ve::log::category::Render, "ImGui Vulkan call failed: " + std::to_string(static_cast<int>(result)));
			}
		}

		void ApplyPresetDefaults(VulkanMinecraftDemoSettings& settings, VulkanMinecraftDemoPreset preset)
		{
			settings.scene.preset = preset;
			switch (preset)
			{
			case VulkanMinecraftDemoPreset::LakesideVillage:
				settings.scene.hill_height = 12;
				settings.scene.house_count = 5;
				settings.scene.tree_count = 28;
				settings.scene.farm_rows = 7;
				settings.scene.water_radius = 11;
				settings.scene.tower_height = 14;
				settings.scene.lantern_count = 12;
				settings.scene.ore_richness = 14;
				settings.scene.market_stall_count = 4;
				settings.scene.floating_island_count = 3;
				settings.scene.ruin_count = 3;
				settings.scene.market = true;
				settings.scene.floating_islands = true;
				settings.scene.ruins = true;
				settings.scene.beacon = true;
				settings.scene.stress_blocks = false;
				break;
			case VulkanMinecraftDemoPreset::QuarryOutpost:
				settings.scene.hill_height = 22;
				settings.scene.house_count = 3;
				settings.scene.tree_count = 12;
				settings.scene.farm_rows = 3;
				settings.scene.water_radius = 6;
				settings.scene.tower_height = 19;
				settings.scene.lantern_count = 18;
				settings.scene.ore_richness = 28;
				settings.scene.market_stall_count = 2;
				settings.scene.floating_island_count = 1;
				settings.scene.ruin_count = 5;
				settings.scene.market = false;
				settings.scene.floating_islands = true;
				settings.scene.ruins = true;
				settings.scene.beacon = true;
				settings.scene.stress_blocks = false;
				break;
			case VulkanMinecraftDemoPreset::ForestFarm:
				settings.scene.hill_height = 9;
				settings.scene.house_count = 4;
				settings.scene.tree_count = 58;
				settings.scene.farm_rows = 12;
				settings.scene.water_radius = 13;
				settings.scene.tower_height = 12;
				settings.scene.lantern_count = 8;
				settings.scene.ore_richness = 8;
				settings.scene.market_stall_count = 3;
				settings.scene.floating_island_count = 4;
				settings.scene.ruin_count = 2;
				settings.scene.market = true;
				settings.scene.floating_islands = true;
				settings.scene.ruins = false;
				settings.scene.beacon = false;
				settings.scene.stress_blocks = false;
				break;
			case VulkanMinecraftDemoPreset::StressTown:
				settings.scene.hill_height = 16;
				settings.scene.house_count = 9;
				settings.scene.tree_count = 72;
				settings.scene.farm_rows = 14;
				settings.scene.water_radius = 16;
				settings.scene.tower_height = 24;
				settings.scene.lantern_count = 28;
				settings.scene.ore_richness = 32;
				settings.scene.market_stall_count = 8;
				settings.scene.floating_island_count = 5;
				settings.scene.ruin_count = 6;
				settings.scene.market = true;
				settings.scene.floating_islands = true;
				settings.scene.ruins = true;
				settings.scene.beacon = true;
				settings.scene.stress_blocks = true;
				break;
			}
			settings.request_scene_rebuild = true;
		}

		void DrawPresetCombo(VulkanMinecraftDemoSettings& settings)
		{
			static constexpr std::array<VulkanMinecraftDemoPreset, 4> presets{ {
				VulkanMinecraftDemoPreset::LakesideVillage,
				VulkanMinecraftDemoPreset::QuarryOutpost,
				VulkanMinecraftDemoPreset::ForestFarm,
				VulkanMinecraftDemoPreset::StressTown
			} };

			const char* current = VulkanMinecraftDemoPresetName(settings.scene.preset);
			if (!ImGui::BeginCombo("Preset", current)) return;
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

		void DrawSceneControls(VulkanMinecraftDemoSettings& settings)
		{
			DrawPresetCombo(settings);
			bool changed = false;
			changed |= ImGui::DragInt("Seed", &settings.scene.seed, 1.0f, 1, 99999);
			changed |= ImGui::DragInt("Ground", &settings.scene.ground_y, 0.35f, 34, 76);
			changed |= ImGui::DragInt("Terrain radius", &settings.scene.terrain_radius, 0.4f, 24, 62);
			changed |= ImGui::DragInt("Hill height", &settings.scene.hill_height, 0.35f, 0, 28);
			changed |= ImGui::DragInt("Houses", &settings.scene.house_count, 0.15f, 0, 9);
			changed |= ImGui::DragInt("Trees", &settings.scene.tree_count, 0.5f, 0, 96);
			changed |= ImGui::DragInt("Farm rows", &settings.scene.farm_rows, 0.25f, 0, 16);
			changed |= ImGui::DragInt("Water radius", &settings.scene.water_radius, 0.25f, 0, 22);
			changed |= ImGui::DragInt("Tower height", &settings.scene.tower_height, 0.25f, 6, 28);
			changed |= ImGui::DragInt("Lanterns", &settings.scene.lantern_count, 0.35f, 0, 32);
			changed |= ImGui::DragInt("Ore blocks", &settings.scene.ore_richness, 0.35f, 0, 32);
			changed |= ImGui::DragInt("Market stalls", &settings.scene.market_stall_count, 0.15f, 0, 8);
			changed |= ImGui::DragInt("Floating islands", &settings.scene.floating_island_count, 0.15f, 0, 6);
			changed |= ImGui::DragInt("Ruins", &settings.scene.ruin_count, 0.15f, 0, 8);
			changed |= ImGui::Checkbox("Village", &settings.scene.village);
			ImGui::SameLine();
			changed |= ImGui::Checkbox("Farms", &settings.scene.farms);
			ImGui::SameLine();
			changed |= ImGui::Checkbox("Lake", &settings.scene.water);
			changed |= ImGui::Checkbox("Cave", &settings.scene.cave);
			ImGui::SameLine();
			changed |= ImGui::Checkbox("Trees", &settings.scene.trees);
			ImGui::SameLine();
			changed |= ImGui::Checkbox("Lights", &settings.scene.lights);
			changed |= ImGui::Checkbox("Market", &settings.scene.market);
			ImGui::SameLine();
			changed |= ImGui::Checkbox("Islands", &settings.scene.floating_islands);
			ImGui::SameLine();
			changed |= ImGui::Checkbox("Ruins", &settings.scene.ruins);
			changed |= ImGui::Checkbox("Beacon", &settings.scene.beacon);
			ImGui::SameLine();
			changed |= ImGui::Checkbox("Stress blocks", &settings.scene.stress_blocks);
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
			if (ImGui::Button("Stress"))
			{
				ApplyPresetDefaults(settings, VulkanMinecraftDemoPreset::StressTown);
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
			ImGui::Text("Indices %u", stats.index_count);
			ImGui::Text("World revision %llu", static_cast<unsigned long long>(stats.world_revision));
			ImGui::Text("Renderer %s", stats.gpu_renderer_active ? "Vulkan GPU" : "CPU fallback");
		}
	}

	bool VulkanImGuiOverlay::Initialize(VulkanBackend& backend, ve::engine::Window& window, VkRenderPass render_pass)
	{
		Release();
		device_ = backend.Device().Handle();
		if (device_ == VK_NULL_HANDLE || render_pass == VK_NULL_HANDLE) return false;

		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		ImGuiIO& io = ImGui::GetIO();
		io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
		ImGui::StyleColorsDark();
		VE_LOG_CATEGORY_INFO(ve::log::category::Render, "Initializing Vulkan ImGui GLFW backend");

		if (!ImGui_ImplGlfw_InitForVulkan(window.GetNativeWindow(), true))
		{
			Release();
			return false;
		}

		const std::uint32_t image_count = static_cast<std::uint32_t>(backend.Swapchain().Images().size());
		VE_LOG_CATEGORY_INFO(ve::log::category::Render, "Initializing Vulkan ImGui renderer backend with swapchain images: " + std::to_string(image_count));
		ImGui_ImplVulkan_InitInfo init_info{};
		init_info.ApiVersion = VK_API_VERSION_1_0;
		init_info.Instance = backend.Context().Instance();
		init_info.PhysicalDevice = backend.PhysicalDevice().Handle();
		init_info.Device = device_;
		init_info.QueueFamily = backend.PhysicalDevice().QueueFamilies().graphics_family;
		init_info.Queue = backend.Device().GraphicsQueue();
		init_info.RenderPass = render_pass;
		init_info.MinImageCount = std::min(std::max(image_count, 2u), 3u);
		init_info.ImageCount = image_count;
		init_info.MSAASamples = VK_SAMPLE_COUNT_1_BIT;
		init_info.DescriptorPoolSize = 64u;
		init_info.CheckVkResultFn = CheckVkResult;
		init_info.MinAllocationSize = 1024u * 1024u;
		if (!ImGui_ImplVulkan_Init(&init_info))
		{
			Release();
			return false;
		}

		initialized_ = true;
		VE_LOG_CATEGORY_INFO(ve::log::category::Render, "Vulkan ImGui overlay initialized");
		return true;
	}

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
			if (ImGui::Begin("Vulkan Minecraft Lab", &settings.show_controls))
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

	void VulkanImGuiOverlay::Record(VkCommandBuffer command_buffer)
	{
		if (!initialized_ || !has_draw_data_) return;
		ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), command_buffer);
	}

	bool VulkanImGuiOverlay::WantsMouseInput() const noexcept
	{
		return initialized_ && ImGui::GetCurrentContext() != nullptr && ImGui::GetIO().WantCaptureMouse;
	}

	bool VulkanImGuiOverlay::IsInitialized() const noexcept
	{
		return initialized_;
	}

	void VulkanImGuiOverlay::Release()
	{
		if (device_ != VK_NULL_HANDLE) vkDeviceWaitIdle(device_);
		if (initialized_)
		{
			ImGui_ImplVulkan_Shutdown();
			ImGui_ImplGlfw_Shutdown();
			ImGui::DestroyContext();
		}
		device_ = VK_NULL_HANDLE;
		initialized_ = false;
		has_draw_data_ = false;
	}
}
