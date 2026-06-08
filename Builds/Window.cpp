#include <utility>
#include "Logger.h"
#include "Window.h"

Window::Window(std::string title)
	: _window(nullptr),
	  _title(std::move(title)),
	  _width(0),
	  _height(0),
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

bool Window::Initialize()
{
	if (glfwInit() != GLFW_TRUE)
	{
		ve::log::Error("Failed to initialize GLFW");
		return false;
	}

	GLFWmonitor* primaryDisplayMonitor = glfwGetPrimaryMonitor();
	const GLFWvidmode* currentVideoDisplayMode = glfwGetVideoMode(primaryDisplayMonitor);


	_width = currentVideoDisplayMode->width;
	_height = currentVideoDisplayMode->height;

	glfwWindowHint(GLFW_DECORATED, GLFW_FALSE);
	glfwWindowHint(GLFW_RED_BITS, currentVideoDisplayMode->redBits);
	glfwWindowHint(GLFW_GREEN_BITS, currentVideoDisplayMode->greenBits);
	glfwWindowHint(GLFW_BLUE_BITS, currentVideoDisplayMode->blueBits);
	glfwWindowHint(GLFW_REFRESH_RATE, currentVideoDisplayMode->refreshRate);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_COMPAT_PROFILE);

	_window = glfwCreateWindow(_width, _height, _title.c_str(), NULL, NULL);
	if (!_window)
	{
		ve::log::Error("Failed to create GLFW window");
		glfwTerminate();
		return false;
	}

	glfwSetWindowUserPointer(_window, &_callbackContext);
	glfwSetFramebufferSizeCallback(_window, FramebufferResizeCallback);
	glfwMakeContextCurrent(_window);
	glewExperimental = GL_TRUE;

	if (glewInit() != GLEW_OK)
	{
		ve::log::Error("Failed to initialize GLEW");
		return false;
	}

	return true;
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
