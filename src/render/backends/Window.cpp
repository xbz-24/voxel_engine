#include "Logger.h"
#include "Window.h"

namespace ve::engine
{
	Window::Window(std::string_view title)
		: _window(nullptr),
		  _width(0),
		  _height(0),
		  _isVSyncEnabled(false),
		  _graphicsApi(ve::rendering::GraphicsApi::Vulkan),
		  _title(title),
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
