#include "Logger.h"
#include "Window.h"
#include "WindowGlfwSession.h"

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

/// Starts GLFW and reports whether initialization succeeded.
bool ve::engine::Window::InitializeGlfw()
{
	if (_ownsGlfwSession) return true;
	if (detail::AcquireGlfwSession())
	{
		_ownsGlfwSession = true;
		return true;
	}
	VE_LOG_ERROR("Failed to initialize GLFW");
	return false;
}

/// Creates the native GLFW window.
bool ve::engine::Window::CreateNativeWindow(GLFWmonitor* fullscreen_monitor)
{
	_window.reset(glfwCreateWindow(_width, _height, _title.c_str(), fullscreen_monitor, nullptr));
	if (_window != nullptr) return true;
	VE_LOG_ERROR("Failed to create GLFW window");
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
	glfwSetWindowUserPointer(_window.get(), &_callbackContext);
	glfwSetFramebufferSizeCallback(_window.get(), FramebufferResizeCallback);
	if (_graphicsApi == ve::rendering::GraphicsApi::OpenGLCompatibility) glfwMakeContextCurrent(_window.get());
}

/// Initializes the native window with the default Vulkan backend.
bool ve::engine::Window::Initialize()
{
	return Initialize(ve::rendering::GraphicsApi::Vulkan);
}

/// Initializes the native window for the requested graphics API.
bool ve::engine::Window::Initialize(ve::rendering::GraphicsApi graphicsApi)
{
	if (_window != nullptr) return _graphicsApi == graphicsApi;
	if (!InitializeGlfw()) return false;
	_graphicsApi = graphicsApi;
	GLFWmonitor* display_monitor = SelectDisplayMonitor();
	const GLFWvidmode* videoMode = ReadDisplayMode(display_monitor);
	if (!videoMode)
	{
		VE_LOG_ERROR("Failed to read GLFW display mode");
		Shutdown();
		return false;
	}
	ApplyWindowHints(*videoMode);
	GLFWmonitor* fullscreen_monitor = _fullscreen ? display_monitor : nullptr;
	if (!CreateNativeWindow(fullscreen_monitor))
	{
		Shutdown();
		return false;
	}
	ConfigureNativeCallbacks();
	ApplyInitialCursorMode();
	return true;
}
