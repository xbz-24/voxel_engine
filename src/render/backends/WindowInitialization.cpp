#include "Logger.h"
#include "Window.h"

/// Starts GLFW and reports whether initialization succeeded.
bool ve::engine::Window::InitializeGlfw()
{
	if (glfwInit() == GLFW_TRUE) return true;
	VE_LOG_ERROR("Failed to initialize GLFW");
	return false;
}

/// Reads the primary monitor video mode and updates window dimensions.
const GLFWvidmode* ve::engine::Window::ReadPrimaryMonitorMode()
{
	GLFWmonitor* primaryDisplayMonitor = glfwGetPrimaryMonitor();
	const GLFWvidmode* videoMode = glfwGetVideoMode(primaryDisplayMonitor);
	if (_fullscreen)
	{
		_width = videoMode->width;
		_height = videoMode->height;
	}
	return videoMode;
}

/// Applies GLFW window hints from a video mode.
void ve::engine::Window::ApplyWindowHints(const GLFWvidmode& videoMode)
{
	glfwWindowHint(GLFW_DECORATED, _fullscreen ? GLFW_FALSE : GLFW_TRUE);
	if (_fullscreen)
	{
		glfwWindowHint(GLFW_RED_BITS, videoMode.redBits);
		glfwWindowHint(GLFW_GREEN_BITS, videoMode.greenBits);
		glfwWindowHint(GLFW_BLUE_BITS, videoMode.blueBits);
		glfwWindowHint(GLFW_REFRESH_RATE, videoMode.refreshRate);
	}
	ApplyGraphicsApiHints();
}

/// Applies API-specific window hints.
void ve::engine::Window::ApplyGraphicsApiHints()
{
	if (_graphicsApi == ve::rendering::GraphicsApi::Vulkan || _graphicsApi == ve::rendering::GraphicsApi::DirectX12)
	{
		glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
		return;
	}
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_COMPAT_PROFILE);
}

/// Creates the native GLFW window.
bool ve::engine::Window::CreateNativeWindow()
{
	_window = glfwCreateWindow(_width, _height, _title.c_str(), nullptr, nullptr);
	if (_window) return true;
	VE_LOG_ERROR("Failed to create GLFW window");
	glfwTerminate();
	return false;
}

/// Wires GLFW user data, callbacks and current context.
void ve::engine::Window::ConfigureNativeCallbacks()
{
	glfwSetWindowUserPointer(_window, &_callbackContext);
	glfwSetFramebufferSizeCallback(_window, FramebufferResizeCallback);
	if (_graphicsApi == ve::rendering::GraphicsApi::OpenGLCompatibility) glfwMakeContextCurrent(_window);
}

/// Initializes the native window with the default Vulkan backend.
bool ve::engine::Window::Initialize()
{
	return Initialize(ve::rendering::GraphicsApi::Vulkan);
}

/// Initializes the native window for the requested graphics API.
bool ve::engine::Window::Initialize(ve::rendering::GraphicsApi graphicsApi)
{
	if (!InitializeGlfw()) return false;
	_graphicsApi = graphicsApi;
	const GLFWvidmode* videoMode = ReadPrimaryMonitorMode();
	ApplyWindowHints(*videoMode);
	if (!CreateNativeWindow()) return false;
	ConfigureNativeCallbacks();
	return true;
}
