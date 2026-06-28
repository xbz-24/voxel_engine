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
		VE_LOG_CATEGORY_INFO(ve::log::category::Render, "Initializing Vulkan ImGui GLFW backend with input callbacks");

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

	void VulkanImGuiOverlay::Record(VkCommandBuffer command_buffer)
	{
		if (!initialized_ || !has_draw_data_) return;
		ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), command_buffer);
	}

	bool VulkanImGuiOverlay::WantsMouseInput() const noexcept
	{
		return initialized_ && ImGui::GetCurrentContext() != nullptr && ImGui::GetIO().WantCaptureMouse;
	}

	bool VulkanImGuiOverlay::WantsKeyboardInput() const noexcept
	{
		if (!initialized_ || ImGui::GetCurrentContext() == nullptr) return false;
		const ImGuiIO& io = ImGui::GetIO();
		return io.WantCaptureKeyboard || io.WantTextInput || ImGui::IsAnyItemActive();
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
