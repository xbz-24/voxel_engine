#include "Window.h"

#include "CoreTypes.h"

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include <cstdint>

void ve::engine::Window::SetVSync(bool isEnabled)
{
	_isVSyncEnabled = isEnabled;
	if (_graphicsApi == ve::rendering::GraphicsApi::OpenGLCompatibility) glfwSwapInterval(isEnabled ? 1 : 0);
}

bool ve::engine::Window::IsVSyncEnabled() const noexcept
{
	return _isVSyncEnabled;
}

void ve::engine::Window::Update()
{
	if (_graphicsApi == ve::rendering::GraphicsApi::OpenGLCompatibility) glfwSwapBuffers(_window.get());
	glfwPollEvents();
}

std::vector<ve::engine::WindowEvent> ve::engine::Window::DrainEvents()
{
	std::vector<WindowEvent> events;
	events.swap(_eventQueue);
	return events;
}

bool ve::engine::Window::ShouldClose() const
{
	return glfwWindowShouldClose(_window.get()) != 0;
}

void ve::engine::Window::Close()
{
	glfwSetWindowShouldClose(_window.get(), GLFW_TRUE);
}

int ve::engine::Window::GetWidth() const
{
	return _width;
}

int ve::engine::Window::GetHeight() const
{
	return _height == 0 ? 1 : _height;
}

float ve::engine::Window::GetAspectRatio() const
{
	return ve::core::ToFloat(_width) / ve::core::ToFloat(GetHeight());
}

ve::engine::Window::WindowSize ve::engine::Window::ClientWindowSize() const noexcept
{
	WindowSize size{};
	if (_window != nullptr)
	{
		glfwGetWindowSize(_window.get(), &size.width, &size.height);
	}
	return size;
}

ve::engine::Window::CursorPosition ve::engine::Window::CurrentCursorPosition() const noexcept
{
	CursorPosition position{};
	if (_window != nullptr)
	{
		glfwGetCursorPos(_window.get(), &position.x, &position.y);
	}
	return position;
}

ve::engine::Window::NativeWindowHandle ve::engine::Window::NativeHandle() const noexcept
{
	return NativeWindowHandle{ _window.get() };
}

GLFWwindow* ve::engine::Window::GetNativeWindow() const
{
	return _window.get();
}

