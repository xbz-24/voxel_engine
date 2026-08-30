#include "Window.h"

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

namespace ve::engine
{
	void Window::MakeGraphicsContextCurrent() noexcept
	{
		if (_window != nullptr &&
			_graphicsApi == ve::rendering::GraphicsApi::OpenGLCompatibility)
		{
			glfwMakeContextCurrent(_window.get());
		}
	}

	void Window::SetVSync(bool isEnabled)
	{
		_isVSyncEnabled = isEnabled;
		if (_graphicsApi == ve::rendering::GraphicsApi::OpenGLCompatibility)
		{
			glfwSwapInterval(isEnabled ? 1 : 0);
		}
	}

	bool Window::IsVSyncEnabled() const noexcept
	{
		return _isVSyncEnabled;
	}

	void Window::Update()
	{
		if (_graphicsApi == ve::rendering::GraphicsApi::OpenGLCompatibility)
		{
			glfwSwapBuffers(_window.get());
		}
		glfwPollEvents();
	}

	std::vector<WindowEvent> Window::DrainEvents()
	{
		return _events.Drain();
	}

	bool Window::ShouldClose() const
	{
		return glfwWindowShouldClose(_window.get()) != 0;
	}

	void Window::Close()
	{
		glfwSetWindowShouldClose(_window.get(), GLFW_TRUE);
	}
}
