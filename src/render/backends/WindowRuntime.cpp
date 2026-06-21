#include "Window.h"

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
	if (_graphicsApi == ve::rendering::GraphicsApi::OpenGLCompatibility) glfwSwapBuffers(_window);
	glfwPollEvents();
}

bool ve::engine::Window::ShouldClose() const
{
	return static_cast<bool>(glfwWindowShouldClose(_window));
}

void ve::engine::Window::Close()
{
	glfwSetWindowShouldClose(_window, GLFW_TRUE);
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
	return static_cast<float>(_width) / static_cast<float>(_height);
}

GLFWwindow* ve::engine::Window::GetNativeWindow() const
{
	return _window;
}

ve::rendering::GraphicsApi ve::engine::Window::GraphicsApi() const noexcept
{
	return _graphicsApi;
}

std::vector<const char*> ve::engine::Window::RequiredVulkanInstanceExtensions() const
{
	std::uint32_t extension_count = 0;
	const char** extensions = glfwGetRequiredInstanceExtensions(&extension_count);
	if (extensions == nullptr) return {};
	return { extensions, extensions + extension_count };
}

void ve::engine::Window::SetCallbackUserData(void* userData)
{
	_callbackContext.userData = userData;
}

void* ve::engine::Window::GetCallbackUserData(GLFWwindow* window)
{
	CallbackContext* context = static_cast<CallbackContext*>(glfwGetWindowUserPointer(window));
	return context ? context->userData : nullptr;
}

void ve::engine::Window::FramebufferResizeCallback(GLFWwindow* window, int width, int height) noexcept
{
	CallbackContext* context = static_cast<CallbackContext*>(glfwGetWindowUserPointer(window));
	if (context && context->window)
	{
		context->window->_width = width;
		context->window->_height = height;
	}
}
