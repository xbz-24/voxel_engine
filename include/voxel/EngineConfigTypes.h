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
		bool asset_search_roots = true;
		bool asset_catalog_runtime_loading = false;
		bool material_library_runtime_binding = false;
		bool runtime_update_callback = true;
		bool scene_graph_authoring = true;
		bool scene_graph_runtime_rendering = false;
		bool embeddable_frame_loop = false;
		bool directx12_runtime_backend = false;
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

	enum class LogLevel
	{
		Default,
		Trace,
		Debug,
		Info,
		Warning,
		Error,
		Fatal
	};

	struct LogSettings
	{
		LogLevel minimum_level = LogLevel::Default;
		bool console_enabled = true;
		bool file_output_enabled = true;
		std::string file_output_path;

		LogSettings& WithMinimumLevel(LogLevel level) noexcept;
		LogSettings& EnableConsole(bool enabled = true) noexcept;
		LogSettings& DisableConsole() noexcept;
		LogSettings& WriteToFile(std::string path);
		LogSettings& DisableFileOutput() noexcept;
	};

	struct WindowConfig
	{
		std::string title = "Voxel Engine";
		int width = 1280;
		int height = 720;
		int monitor_index = 0;
		int refresh_rate_hertz = 0;
		bool fullscreen = false;
		bool vsync = false;
		bool resizable = true;
		bool high_dpi_framebuffer = true;
		bool capture_cursor_on_start = false;

		[[nodiscard]] static WindowConfig Windowed(std::string title, int width = 1280, int height = 720);
		[[nodiscard]] static WindowConfig Fullscreen(std::string title = "Voxel Engine");

		WindowConfig& EnableVSync(bool enabled = true) noexcept;
		WindowConfig& UseMonitor(int index) noexcept;
		WindowConfig& WithRefreshRate(int hertz) noexcept;
		WindowConfig& SetResizable(bool enabled = true) noexcept;
		WindowConfig& EnableHighDpiFramebuffer(bool enabled = true) noexcept;
		WindowConfig& CaptureCursorOnStart(bool enabled = true) noexcept;
	};

	[[nodiscard]] WindowConfig Window(std::string title, int width = 1280, int height = 720);
	[[nodiscard]] WindowConfig Fullscreen(std::string title = "Voxel Engine");
}
