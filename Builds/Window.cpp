#include "Logger.h"
#include "Window.h"

#include <utility>

Window::Window(std::string title)
	: _window(nullptr),
	  _title(std::move(title)),
	  _width(0),
	  _height(0),
	  _isVSyncEnabled(false),
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

/// Starts GLFW and reports whether initialization succeeded.
bool Window::InitializeGlfw()
{
	if (glfwInit() == GLFW_TRUE)
	{
		return true;
	}
	VE_LOG_ERROR("Failed to initialize GLFW");
	return false;
}

/// Reads the primary monitor video mode and updates window dimensions.
const GLFWvidmode* Window::ReadPrimaryMonitorMode()
{
	GLFWmonitor* primaryDisplayMonitor = glfwGetPrimaryMonitor();
	const GLFWvidmode* videoMode = glfwGetVideoMode(primaryDisplayMonitor);
	_width = videoMode->width;
	_height = videoMode->height;
	return videoMode;
}

/// Applies GLFW window hints from a video mode.
void Window::ApplyWindowHints(const GLFWvidmode& videoMode)
{
	glfwWindowHint(GLFW_DECORATED, GLFW_FALSE);
	glfwWindowHint(GLFW_RED_BITS, videoMode.redBits);
	glfwWindowHint(GLFW_GREEN_BITS, videoMode.greenBits);
	glfwWindowHint(GLFW_BLUE_BITS, videoMode.blueBits);
	glfwWindowHint(GLFW_REFRESH_RATE, videoMode.refreshRate);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_COMPAT_PROFILE);
}

/// Creates the native GLFW window.
bool Window::CreateNativeWindow()
{
	_window = glfwCreateWindow(_width, _height, _title.c_str(), nullptr, nullptr);
	if (_window) return true;
	VE_LOG_ERROR("Failed to create GLFW window");
	glfwTerminate();
	return false;
}

/// Wires GLFW user data, callbacks and current context.
void Window::ConfigureNativeCallbacks()
{
	glfwSetWindowUserPointer(_window, &_callbackContext);
	glfwSetFramebufferSizeCallback(_window, FramebufferResizeCallback);
	glfwMakeContextCurrent(_window);
}

/// Initializes GLEW after the OpenGL context exists.
bool Window::InitializeOpenGLLoader()
{
	glewExperimental = GL_TRUE;
	if (glewInit() == GLEW_OK) return true;
	VE_LOG_ERROR("Failed to initialize GLEW");
	return false;
}

/// Initializes the native window and OpenGL loader.
bool Window::Initialize()
{
	if (!InitializeGlfw()) return false;
	const GLFWvidmode* videoMode = ReadPrimaryMonitorMode();
	ApplyWindowHints(*videoMode);
	if (!CreateNativeWindow()) return false;
	ConfigureNativeCallbacks();
	return InitializeOpenGLLoader();
}
