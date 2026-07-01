#pragma once

#include "voxel/Assets.h"
#include "voxel/Camera.h"
#include "voxel/EngineConfigTypes.h"
#include "voxel/Materials.h"
#include "voxel/Runtime.h"
#include "voxel/Scene.h"
#include "voxel/World.h"

#include <string>
#include <vector>

namespace voxel
{
	struct EngineConfig
	{
		// TODO: Split this into lightweight startup config and runtime-editable settings once hot reload is supported.
		WindowConfig window{};
		WorldConfig world{};
		AssetCatalog assets{};
		MaterialLibrary materials{};
		SceneGraph scene_graph{};
		CameraConfig camera{};
		LogSettings logging{};
		GraphicsApi graphics_api = GraphicsApi::Vulkan;
		DemoScene demo_scene = DemoScene::HyperrealDesert;
		int render_distance_chunks = 8;
		bool show_debug_overlay = true;
		UpdateCallback on_update{};
		DiagnosticsCallback on_diagnostics{};
		LogCallback on_log{};

		[[nodiscard]] static EngineConfig Default();
		[[nodiscard]] static EngineConfig DesertDemo();
		[[nodiscard]] static EngineConfig AquaModelDemo();
		[[nodiscard]] static EngineConfig SponzaAtriumDemo();
		[[nodiscard]] static EngineConfig MinecraftDemo();
		[[nodiscard]] static EngineConfig WorldEditDemo();
		[[nodiscard]] static EngineConfig ShowcaseDemo();
		[[nodiscard]] static EngineConfig ArcadeSnakeDemo();
		[[nodiscard]] static EngineConfig ArcadePaddleDemo();
		[[nodiscard]] static EngineConfig ArcadeBlocksDemo();
		[[nodiscard]] static EngineConfig ArcadeInvadersDemo();
		[[nodiscard]] static EngineConfig ArcadeMazeDemo();
		[[nodiscard]] std::vector<std::string> Validate() const;
		[[nodiscard]] bool IsValid() const;

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
		EngineConfig& WithDemoScene(DemoScene value) noexcept;
		EngineConfig& WithWorldSizeChunks(int value) noexcept;
		EngineConfig& WithRenderDistanceChunks(int value) noexcept;
		EngineConfig& ShowDebugOverlay(bool enabled = true) noexcept;
		EngineConfig& HideDebugOverlay() noexcept;
		EngineConfig& OnUpdate(UpdateCallback callback) noexcept;
		EngineConfig& OnDiagnostics(DiagnosticsCallback callback) noexcept;
		EngineConfig& OnLog(LogCallback callback) noexcept;
	};
}
