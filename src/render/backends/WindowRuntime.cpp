#include "Window.h"

#include <cstdint>

void Window::SetVSync(bool isEnabled)
{
	_isVSyncEnabled = isEnabled;
	if (_graphicsApi == ve::rendering::GraphicsApi::OpenGLCompatibility) glfwSwapInterval(isEnabled ? 1 : 0);
}

bool Window::IsVSyncEnabled() const noexcept
{
	return _isVSyncEnabled;
}

void Window::Update()
{
	if (_graphicsApi == ve::rendering::GraphicsApi::OpenGLCompatibility) glfwSwapBuffers(_window);
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

void Window::SetCallbackUserData(void* userData)
{
	_callbackContext.userData = userData;
}

void* Window::GetCallbackUserData(GLFWwindow* window)
{
	CallbackContext* context = static_cast<CallbackContext*>(glfwGetWindowUserPointer(window));
	return context ? context->userData : nullptr;
}

void Window::FramebufferResizeCallback(GLFWwindow* window, int width, int height) noexcept
{
	CallbackContext* context = static_cast<CallbackContext*>(glfwGetWindowUserPointer(window));
	if (context && context->window)
	{
		context->window->_width = width;
		context->window->_height = height;
	}
}
