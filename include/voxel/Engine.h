#pragma once

#include "voxel/Camera.h"
#include "voxel/World.h"

#include <memory>
#include <string>

namespace voxel
{
	enum class GraphicsApi
	{
		Vulkan,
		OpenGLCompatibility,
		DirectX12
	};

	struct WindowConfig
	{
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
		WindowConfig window{};
		WorldConfig world{};
		CameraConfig camera{};
		GraphicsApi graphics_api = GraphicsApi::Vulkan;
		int render_distance_chunks = 2;
		bool show_debug_overlay = true;

		[[nodiscard]] static EngineConfig Default();
		[[nodiscard]] static EngineConfig MinecraftDemo();
		[[nodiscard]] static EngineConfig WorldEditDemo();
		[[nodiscard]] static EngineConfig ShowcaseDemo();

		EngineConfig& WithWindow(WindowConfig value) noexcept;
		EngineConfig& WithWindow(std::string title, int width = 1280, int height = 720);
		EngineConfig& WithWorld(WorldConfig value) noexcept;
		EngineConfig& WithScene(WorldConfig value) noexcept;
		EngineConfig& WithCamera(CameraConfig value) noexcept;
		EngineConfig& WithCamera(Vec3 position, Vec3 look_at) noexcept;
		EngineConfig& UseGraphicsApi(GraphicsApi value) noexcept;
		EngineConfig& UseVulkan() noexcept;
		EngineConfig& UseOpenGLCompatibility() noexcept;
		EngineConfig& UseDirectX12() noexcept;
		EngineConfig& WithWorldSizeChunks(int value) noexcept;
		EngineConfig& WithRenderDistanceChunks(int value) noexcept;
		EngineConfig& ShowDebugOverlay(bool enabled = true) noexcept;
		EngineConfig& HideDebugOverlay() noexcept;
	};

	[[nodiscard]] EngineConfig MinecraftDemo();
	[[nodiscard]] EngineConfig WorldEditDemo();
	[[nodiscard]] EngineConfig ShowcaseDemo();
	[[nodiscard]] EngineConfig DefaultConfig();

	class Engine
	{
	public:
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
	[[nodiscard]] int RunMinecraftDemo();
	[[nodiscard]] int RunWorldEditDemo();
	[[nodiscard]] int RunShowcaseDemo();
}
