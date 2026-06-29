#pragma once

#include <string>

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
		// TODO: Expose monitor/refresh-rate selection for real fullscreen support.
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
}
