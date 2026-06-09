#include "Logger.h"
#include "Window.h"

#include <utility>

Window::Window(std::string title)
	: _window(nullptr),
	  _title(std::move(title)),
	  _width(0),
	  _height(0),
	  _isVSyncEnabled(false),
	  _graphicsApi(ve::rendering::GraphicsApi::OpenGLCompatibility),
	  _callbackContext{ this, nullptr }
{
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
