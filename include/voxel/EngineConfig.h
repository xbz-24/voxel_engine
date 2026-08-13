#pragma once

#include "voxel/EngineConfigSlices.h"
#include "voxel/Runtime.h"

#include <string>
#include <vector>

namespace voxel
{
	/** Main fluent configuration object for launching or embedding the engine. */
	struct EngineConfig
	{
		WindowConfig window{};
		WorldConfig world{};
		AssetCatalog assets{};
		MaterialLibrary materials{};
		SceneGraph scene_graph{};
		CameraConfig camera{};
		LogSettings logging{};
		GraphicsApi graphics_api = GraphicsApi::Vulkan;
		VoxelRenderStyle voxel_render_style{};
		int render_distance_chunks = 8;
		bool show_debug_overlay = true;
		bool enable_settings_menu = true;
		UpdateCallback on_update{};
		DiagnosticsCallback on_diagnostics{};
		LogCallback on_log{};

		[[nodiscard]] static EngineConfig Default();
		[[nodiscard]] std::vector<std::string> Validate() const;
		[[nodiscard]] bool IsValid() const;
		[[nodiscard]] EngineStartupConfig StartupConfig() const;
		[[nodiscard]] EngineRuntimeTuning RuntimeTuning() const noexcept;

		EngineConfig& WithWindow(WindowConfig value) noexcept;
		EngineConfig& WithWindow(std::string title, int width = 1280, int height = 720);
		EngineConfig& WithWorld(WorldConfig value) noexcept;
		EngineConfig& WithScene(WorldConfig value) noexcept;
		EngineConfig& WithAssets(AssetCatalog value) noexcept;
		EngineConfig& WithMaterials(MaterialLibrary value) noexcept;
		EngineConfig& WithSceneGraph(SceneGraph value) noexcept;
		EngineConfig& WithCamera(CameraConfig value) noexcept;
		EngineConfig& WithCamera(Vec3 position, Vec3 look_at) noexcept;
		EngineConfig& WithLogging(LogSettings value) noexcept;
		EngineConfig& WithLogLevel(LogLevel value) noexcept;
		EngineConfig& LogToFile(std::string path);
		EngineConfig& DisableFileLogging() noexcept;
		EngineConfig& EnableConsoleLogging(bool enabled = true) noexcept;
		EngineConfig& UseGraphicsApi(GraphicsApi value) noexcept;
		EngineConfig& UseVulkan() noexcept;
		EngineConfig& UseOpenGLCompatibility() noexcept;
		EngineConfig& UseDirectX12() noexcept;
		EngineConfig& WithVoxelRenderStyle(VoxelRenderStyle value) noexcept;
		EngineConfig& WithWorldSizeChunks(int value) noexcept;
		EngineConfig& WithRenderDistanceChunks(int value) noexcept;
		EngineConfig& WithRuntimeTuning(EngineRuntimeTuning value) noexcept;
		EngineConfig& ShowDebugOverlay(bool enabled = true) noexcept;
		EngineConfig& HideDebugOverlay() noexcept;
		EngineConfig& EnableSettingsMenu(bool enabled = true) noexcept;
		EngineConfig& DisableSettingsMenu() noexcept;
		EngineConfig& OnUpdate(UpdateCallback callback) noexcept;
		EngineConfig& OnDiagnostics(DiagnosticsCallback callback) noexcept;
		EngineConfig& OnLog(LogCallback callback) noexcept;
	};
}
