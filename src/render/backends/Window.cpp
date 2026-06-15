#include "Logger.h"
#include "Window.h"

#include <utility>

Window::Window(std::string title)
	: _window(nullptr),
	  _width(0),
	  _height(0),
	  _isVSyncEnabled(false),
	  _graphicsApi(ve::rendering::GraphicsApi::Vulkan),
	  _title(std::move(title)),
	  _callbackContext{ nullptr, nullptr }
{
	_callbackContext.window = this;
}

Window::Window(const char* title) : Window(std::string(title)) {}
Window::~Window()
{
	if (_window)
	{
		glfwDestroyWindow(_window);
	}
	glfwTerminate();
}
