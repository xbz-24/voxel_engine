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
		  _isVSyncEnabled(false),
		  _graphicsApi(ve::rendering::GraphicsApi::Vulkan),
		  _title(std::move(create_info.title)),
		  _fullscreen(create_info.fullscreen),
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
