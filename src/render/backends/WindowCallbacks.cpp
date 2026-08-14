#include "Window.h"

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

namespace ve::engine
{
	void Window::SetCallbackUserData(void* userData) noexcept
	{
		_callbackContext.userData = userData;
	}

	Window::CallbackContext* Window::GetCallbackContext(GLFWwindow* window) noexcept
	{
		return static_cast<CallbackContext*>(glfwGetWindowUserPointer(window));
	}

	void* Window::GetCallbackUserData(GLFWwindow* window) noexcept
	{
		CallbackContext* context = GetCallbackContext(window);
		return context != nullptr ? context->userData : nullptr;
	}

	void Window::RecordFramebufferResize(int width, int height) noexcept
	{
		_width = width;
		_height = height;
		_events.RecordFramebufferResize(width, height);
	}

	void Window::FramebufferResizeCallback(
		GLFWwindow* window,
		int width,
		int height) noexcept
	{
		CallbackContext* context = GetCallbackContext(window);
		if (context != nullptr && context->window != nullptr)
		{
			context->window->RecordFramebufferResize(width, height);
		}
	}
}
