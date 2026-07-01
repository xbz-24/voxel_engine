#pragma once

#include <string>

namespace ve::engine
{
	struct WindowCreateInfo
	{
		std::string title = "Voxel Engine v1.0.0";
		int width = 1280;
		int height = 720;
		int display_index = 0;
		int refresh_rate_hertz = 0;
		bool fullscreen = false;
		bool resizable = true;
		bool high_dpi_framebuffer = true;
		bool capture_cursor_on_start = false;
	};
}
