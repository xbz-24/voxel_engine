#include "Logger.h"
#include "Window.h"

#include <utility>

namespace ve::engine
{
	Window::Window(std::string_view title)
		: Window(WindowCreateInfo{ .title = std::string{ title }, .fullscreen = true })
	{
	}

	Window::Window(WindowCreateInfo create_info)
		: _window(nullptr),
		  _width(create_info.width),
		  _height(create_info.height),
		  _displayIndex(create_info.display_index),
		  _refreshRateHertz(create_info.refresh_rate_hertz),
		  _isVSyncEnabled(false),
		  _graphicsApi(ve::rendering::GraphicsApi::Vulkan),
		  _title(std::move(create_info.title)),
		  _fullscreen(create_info.fullscreen),
		  _resizable(create_info.resizable),
		  _highDpiFramebuffer(create_info.high_dpi_framebuffer),
		  _captureCursorOnStart(create_info.capture_cursor_on_start),
		  _callbackContext{ nullptr, nullptr }
	{
		_callbackContext.window = this;
	}
	Window::~Window()
	{
		if (_window)
		{
			glfwDestroyWindow(_window);
		}
		glfwTerminate();
	}
}
