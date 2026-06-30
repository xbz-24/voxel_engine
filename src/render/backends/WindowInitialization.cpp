#include "Logger.h"
#include "Window.h"

/// Starts GLFW and reports whether initialization succeeded.
bool ve::engine::Window::InitializeGlfw()
{
	if (glfwInit() == GLFW_TRUE) return true;
	VE_LOG_ERROR("Failed to initialize GLFW");
	return false;
}

/// Creates the native GLFW window.
bool ve::engine::Window::CreateNativeWindow(GLFWmonitor* fullscreen_monitor)
{
	_window = glfwCreateWindow(_width, _height, _title.c_str(), fullscreen_monitor, nullptr);
	if (_window) return true;
	VE_LOG_ERROR("Failed to create GLFW window");
	glfwTerminate();
	return false;
}

/// Applies startup-only window input state.
void ve::engine::Window::ApplyInitialCursorMode()
{
	if (_captureCursorOnStart) SetCursorMode(CursorMode::Captured);
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
	GLFWmonitor* display_monitor = SelectDisplayMonitor();
	const GLFWvidmode* videoMode = ReadDisplayMode(display_monitor);
	if (!videoMode)
	{
		VE_LOG_ERROR("Failed to read GLFW display mode");
		glfwTerminate();
		return false;
	}
	ApplyWindowHints(*videoMode);
	GLFWmonitor* fullscreen_monitor = _fullscreen ? display_monitor : nullptr;
	if (!CreateNativeWindow(fullscreen_monitor)) return false;
	ConfigureNativeCallbacks();
	ApplyInitialCursorMode();
	return true;
}
