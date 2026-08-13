#pragma once

#include <string>

namespace voxel
{
	/** Window creation settings used by EngineConfig and host applications. */
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
