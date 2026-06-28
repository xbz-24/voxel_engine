#pragma once

#include "voxel/Assets.h"
#include "voxel/Camera.h"
#include "voxel/Materials.h"
#include "voxel/Runtime.h"
#include "voxel/Scene.h"
#include "voxel/World.h"

#include <memory>
#include <string>
#include <vector>

namespace voxel
{
	struct SdkVersion
	{
		int major = 0;
		int minor = 1;
		int patch = 0;
	};

	struct SdkFeatures
	{
		bool vulkan_by_default = true;
		bool world_config_serialization = true;
		bool runtime_update_callback = true;
		bool scene_graph_authoring = false;
	};

	[[nodiscard]] constexpr SdkVersion Version() noexcept
	{
		return {};
	}

	[[nodiscard]] constexpr SdkFeatures Features() noexcept
	{
		return {};
	}

	enum class GraphicsApi
	{
		Vulkan,
		OpenGLCompatibility,
		DirectX12
	};

	enum class DemoScene
	{
		HyperrealDesert,
		AquaModel,
		SponzaAtrium
	};

	struct WindowConfig
	{
		// TODO: Validate public window dimensions and expose monitor/refresh-rate selection for real fullscreen support.
		std::string title = "Voxel Engine";
		int width = 1280;
		int height = 720;
		bool fullscreen = false;
		bool vsync = false;

		[[nodiscard]] static WindowConfig Windowed(std::string title, int width = 1280, int height = 720);
		[[nodiscard]] static WindowConfig Fullscreen(std::string title = "Voxel Engine");

		WindowConfig& EnableVSync(bool enabled = true) noexcept;
	};

	[[nodiscard]] WindowConfig Window(std::string title, int width = 1280, int height = 720);
	[[nodiscard]] WindowConfig Fullscreen(std::string title = "Voxel Engine");

	struct EngineConfig
	{
		// TODO: Split this into lightweight startup config and runtime-editable settings once hot reload is supported.
		WindowConfig window{};
		WorldConfig world{};
		AssetCatalog assets{};
		MaterialLibrary materials{};
		SceneGraph scene_graph{};
		CameraConfig camera{};
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

	[[nodiscard]] EngineConfig DesertDemo();
	[[nodiscard]] EngineConfig AquaModelDemo();
	[[nodiscard]] EngineConfig SponzaAtriumDemo();
	[[nodiscard]] EngineConfig MinecraftDemo();
	[[nodiscard]] EngineConfig WorldEditDemo();
	[[nodiscard]] EngineConfig ShowcaseDemo();
	[[nodiscard]] EngineConfig ArcadeSnakeDemo();
	[[nodiscard]] EngineConfig ArcadePaddleDemo();
	[[nodiscard]] EngineConfig ArcadeBlocksDemo();
	[[nodiscard]] EngineConfig ArcadeInvadersDemo();
	[[nodiscard]] EngineConfig ArcadeMazeDemo();
	[[nodiscard]] EngineConfig DefaultConfig();

	class Engine
	{
	public:
		// TODO: Expose Pause/Step/Stop hooks so embedding apps can drive the engine without surrendering the main loop.
		explicit Engine(EngineConfig config = {});
		~Engine();

		Engine(const Engine&) = delete;
		Engine& operator=(const Engine&) = delete;
		Engine(Engine&&) noexcept;
		Engine& operator=(Engine&&) noexcept;

		[[nodiscard]] int Run();

	private:
		class Impl;
		std::unique_ptr<Impl> impl_;
	};

	[[nodiscard]] int Run(EngineConfig config = {});
	[[nodiscard]] int Run(WorldConfig world);
	[[nodiscard]] int RunScene(WorldConfig scene);
	[[nodiscard]] int RunDesertDemo();
	[[nodiscard]] int RunAquaModelDemo();
	[[nodiscard]] int RunSponzaAtriumDemo();
	[[nodiscard]] int RunMinecraftDemo();
	[[nodiscard]] int RunWorldEditDemo();
	[[nodiscard]] int RunShowcaseDemo();
	[[nodiscard]] int RunArcadeSnakeDemo();
	[[nodiscard]] int RunArcadePaddleDemo();
	[[nodiscard]] int RunArcadeBlocksDemo();
	[[nodiscard]] int RunArcadeInvadersDemo();
	[[nodiscard]] int RunArcadeMazeDemo();
}
