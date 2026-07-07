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
	/** Startup-only settings extracted from EngineConfig before runtime systems are created. */
	struct EngineStartupConfig
	{
		WindowConfig window{};
		WorldConfig world{};
		AssetCatalog assets{};
		MaterialLibrary materials{};
		SceneGraph scene_graph{};
		CameraConfig camera{};
		LogSettings logging{};
		GraphicsApi graphics_api = GraphicsApi::Vulkan;
		DemoScene demo_scene = DemoScene::HyperrealDesert;
	};

	/** Per-frame runtime tuning extracted from EngineConfig after startup validation. */
	struct EngineRuntimeTuning
	{
		int render_distance_chunks = 8;
		bool show_debug_overlay = true;
		bool enable_settings_menu = true;
	};

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
		DemoScene demo_scene = DemoScene::HyperrealDesert;
		int render_distance_chunks = 8;
		bool show_debug_overlay = true;
		bool enable_settings_menu = true;
		UpdateCallback on_update{};
		DiagnosticsCallback on_diagnostics{};
		LogCallback on_log{};

		/** Returns the default runtime configuration. */
		[[nodiscard]] static EngineConfig Default();

		/** Returns the authored hyperreal voxel desert demo configuration. */
		[[nodiscard]] static EngineConfig DesertDemo();

		/** Returns the Aqua model voxelization demo configuration. */
		[[nodiscard]] static EngineConfig AquaModelDemo();

		/** Returns the Sponza atrium voxelization demo configuration. */
		[[nodiscard]] static EngineConfig SponzaAtriumDemo();

		/** Returns the legacy Minecraft-style demo configuration. */
		[[nodiscard]] static EngineConfig MinecraftDemo();

		/** Returns the world-editing callback demo configuration. */
		[[nodiscard]] static EngineConfig WorldEditDemo();

		/** Returns the general showcase demo configuration. */
		[[nodiscard]] static EngineConfig ShowcaseDemo();

		/** Returns the arcade snake demo configuration. */
		[[nodiscard]] static EngineConfig ArcadeSnakeDemo();

		/** Returns the arcade paddle demo configuration. */
		[[nodiscard]] static EngineConfig ArcadePaddleDemo();

		/** Returns the arcade blocks demo configuration. */
		[[nodiscard]] static EngineConfig ArcadeBlocksDemo();

		/** Returns the arcade invaders demo configuration. */
		[[nodiscard]] static EngineConfig ArcadeInvadersDemo();

		/** Returns the arcade maze demo configuration. */
		[[nodiscard]] static EngineConfig ArcadeMazeDemo();

		/** Returns human-readable validation issues without mutating the config. */
		[[nodiscard]] std::vector<std::string> Validate() const;

		/** Returns true when Validate() would produce no issues. */
		[[nodiscard]] bool IsValid() const;

		/** Returns the startup slice consumed before the frame loop exists. */
		[[nodiscard]] EngineStartupConfig StartupConfig() const;

		/** Returns the runtime-tuning slice consumed after startup. */
		[[nodiscard]] EngineRuntimeTuning RuntimeTuning() const noexcept;

		/** Replaces the window creation settings. */
		EngineConfig& WithWindow(WindowConfig value) noexcept;

		/** Replaces the window settings with a windowed title and size. */
		EngineConfig& WithWindow(std::string title, int width = 1280, int height = 720);

		/** Replaces the world generation and edit settings. */
		EngineConfig& WithWorld(WorldConfig value) noexcept;

		/** Alias for WithWorld when the config is used as authored scene content. */
		EngineConfig& WithScene(WorldConfig value) noexcept;

		/** Replaces the asset catalog. */
		EngineConfig& WithAssets(AssetCatalog value) noexcept;

		/** Replaces the material library. */
		EngineConfig& WithMaterials(MaterialLibrary value) noexcept;

		/** Replaces the scene graph request. */
		EngineConfig& WithSceneGraph(SceneGraph value) noexcept;

		/** Replaces the camera startup settings. */
		EngineConfig& WithCamera(CameraConfig value) noexcept;

		/** Sets a custom startup camera by position and look-at target. */
		EngineConfig& WithCamera(Vec3 position, Vec3 look_at) noexcept;

		/** Replaces logging settings. */
		EngineConfig& WithLogging(LogSettings value) noexcept;

		/** Sets the minimum log severity. */
		EngineConfig& WithLogLevel(LogLevel value) noexcept;

		/** Enables file logging to path. */
		EngineConfig& LogToFile(std::string path);

		/** Disables file logging. */
		EngineConfig& DisableFileLogging() noexcept;

		/** Enables or disables console logging. */
		EngineConfig& EnableConsoleLogging(bool enabled = true) noexcept;

		/** Requests a specific graphics backend. */
		EngineConfig& UseGraphicsApi(GraphicsApi value) noexcept;

		/** Requests the Vulkan backend. */
		EngineConfig& UseVulkan() noexcept;

		/** Requests the OpenGL compatibility backend. */
		EngineConfig& UseOpenGLCompatibility() noexcept;

		/** Requests the DirectX 12 backend when available. */
		EngineConfig& UseDirectX12() noexcept;

		/** Selects the built-in demo scene used by demo presets. */
		EngineConfig& WithDemoScene(DemoScene value) noexcept;

		/** Sets square world size in chunks. */
		EngineConfig& WithWorldSizeChunks(int value) noexcept;

		/** Sets render distance in chunks around the active camera. */
		EngineConfig& WithRenderDistanceChunks(int value) noexcept;

		/** Replaces runtime tuning options. */
		EngineConfig& WithRuntimeTuning(EngineRuntimeTuning value) noexcept;

		/** Enables or disables the debug overlay. */
		EngineConfig& ShowDebugOverlay(bool enabled = true) noexcept;

		/** Disables the debug overlay. */
		EngineConfig& HideDebugOverlay() noexcept;

		/** Enables or disables the in-game settings menu. */
		EngineConfig& EnableSettingsMenu(bool enabled = true) noexcept;

		/** Disables the in-game settings menu. */
		EngineConfig& DisableSettingsMenu() noexcept;

		/** Installs a callback invoked once per frame before commands are applied. */
		EngineConfig& OnUpdate(UpdateCallback callback) noexcept;

		/** Installs a callback that receives runtime diagnostics snapshots. */
		EngineConfig& OnDiagnostics(DiagnosticsCallback callback) noexcept;

		/** Installs a callback that receives runtime log lines. */
		EngineConfig& OnLog(LogCallback callback) noexcept;
	};
}
