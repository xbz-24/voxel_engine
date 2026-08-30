#include "Window.h"

#include "CoreTypes.h"

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include <cstdint>

namespace ve::engine
{
	int Window::GetWidth() const { return _width; }

	int Window::GetHeight() const
	{
		return _height == 0 ? 1 : _height;
	}

	float Window::GetAspectRatio() const
	{
		return ve::core::ToFloat(_width) / ve::core::ToFloat(GetHeight());
	}

	Window::WindowSize Window::ClientWindowSize() const noexcept
	{
		WindowSize size{};
		if (_window != nullptr)
		{
			glfwGetWindowSize(_window.get(), &size.width, &size.height);
		}
		return size;
	}

	Window::WindowSize Window::FramebufferSize() const noexcept
	{
		WindowSize size{ 0, 0 };
		if (_window != nullptr)
		{
			glfwGetFramebufferSize(_window.get(), &size.width, &size.height);
		}
		return size;
	}

	Window::CursorPosition Window::CurrentCursorPosition() const noexcept
	{
		CursorPosition position{};
		if (_window != nullptr)
		{
			glfwGetCursorPos(_window.get(), &position.x, &position.y);
		}
		return position;
	}

	Window::NativeWindowHandle Window::NativeHandle() const noexcept
	{
		return NativeWindowHandle{ _window.get() };
	}

	GLFWwindow* Window::GetNativeWindow() const { return _window.get(); }

	ve::rendering::GraphicsApi Window::GraphicsApi() const noexcept
	{
		return _graphicsApi;
	}

	std::vector<const char*> Window::RequiredVulkanInstanceExtensions() const
	{
		std::uint32_t extension_count = 0;
		const char** extensions = glfwGetRequiredInstanceExtensions(&extension_count);
		if (extensions == nullptr) return {};
		return { extensions, extensions + extension_count };
	}
}
