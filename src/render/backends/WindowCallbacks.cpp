#include "Window.h"

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

namespace ve::engine
{
	void Window::SetCallbackUserData(void* userData)
	{
		_callbackContext.userData = userData;
	}

	Window::CallbackContext* Window::GetCallbackContext(GLFWwindow* window)
	{
		return static_cast<CallbackContext*>(glfwGetWindowUserPointer(window));
	}

	void* Window::GetCallbackUserData(GLFWwindow* window)
	{
		CallbackContext* context = GetCallbackContext(window);
		return context != nullptr ? context->userData : nullptr;
	}

	void Window::RecordFramebufferResize(int width, int height)
	{
		_width = width;
		_height = height;
		_eventQueue.push_back(WindowEvent{
			.kind = WindowEvent::Kind::FramebufferResized,
			.framebuffer_resized = WindowFramebufferResizeEvent{ width, height }
		});
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
