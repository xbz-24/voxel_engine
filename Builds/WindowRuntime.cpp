#include "Window.h"

void Window::SetVSync(bool isEnabled)
{
	_isVSyncEnabled = isEnabled;
	glfwSwapInterval(isEnabled ? 1 : 0);
}

bool Window::IsVSyncEnabled() const noexcept
{
	return _isVSyncEnabled;
}

void Window::Update()
{
	glfwSwapBuffers(_window);
	glfwPollEvents();
}

bool Window::ShouldClose() const
{
	return static_cast<bool>(glfwWindowShouldClose(_window));
}

void Window::Close()
{
	glfwSetWindowShouldClose(_window, GLFW_TRUE);
}

int Window::GetWidth() const
{
	return _width;
}

int Window::GetHeight() const
{
	return _height == 0 ? 1 : _height;
}

float Window::GetAspectRatio() const
{
	return static_cast<float>(_width) / static_cast<float>(_height);
}

GLFWwindow* Window::GetNativeWindow() const
{
	return _window;
}

void Window::SetCallbackUserData(void* userData)
{
	_callbackContext.userData = userData;
}

void* Window::GetCallbackUserData(GLFWwindow* window)
{
	CallbackContext* context = static_cast<CallbackContext*>(glfwGetWindowUserPointer(window));
	return context ? context->userData : nullptr;
}

void Window::FramebufferResizeCallback(GLFWwindow* window, int width, int height)
{
	CallbackContext* context = static_cast<CallbackContext*>(glfwGetWindowUserPointer(window));
	if (context && context->window)
	{
		context->window->_width = width;
		context->window->_height = height;
	}
}
