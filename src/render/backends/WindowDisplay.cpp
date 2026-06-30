#include "Window.h"

#include <algorithm>

/// Selects the configured GLFW display, falling back to the primary monitor.
GLFWmonitor* ve::engine::Window::SelectDisplayMonitor()
{
	int display_count = 0;
	GLFWmonitor** displays = glfwGetMonitors(&display_count);
	if (!displays || display_count <= 0) return glfwGetPrimaryMonitor();

	const int display_index = std::clamp(_displayIndex, 0, display_count - 1);
	return displays[display_index] ? displays[display_index] : glfwGetPrimaryMonitor();
}

/// Reads the selected display mode and sizes fullscreen windows to it.
const GLFWvidmode* ve::engine::Window::ReadDisplayMode(GLFWmonitor* display_monitor)
{
	if (!display_monitor) return nullptr;

	const GLFWvidmode* video_mode = glfwGetVideoMode(display_monitor);
	if (_fullscreen && video_mode)
	{
		_width = video_mode->width;
		_height = video_mode->height;
	}
	return video_mode;
}

/// Applies GLFW window hints from the selected display mode.
void ve::engine::Window::ApplyWindowHints(const GLFWvidmode& videoMode)
{
	glfwWindowHint(GLFW_DECORATED, _fullscreen ? GLFW_FALSE : GLFW_TRUE);
	glfwWindowHint(GLFW_RESIZABLE, _resizable ? GLFW_TRUE : GLFW_FALSE);
#ifdef GLFW_SCALE_TO_MONITOR
	glfwWindowHint(GLFW_SCALE_TO_MONITOR, _highDpiFramebuffer ? GLFW_TRUE : GLFW_FALSE);
#endif
#ifdef GLFW_COCOA_RETINA_FRAMEBUFFER
	glfwWindowHint(GLFW_COCOA_RETINA_FRAMEBUFFER, _highDpiFramebuffer ? GLFW_TRUE : GLFW_FALSE);
#endif
	if (_fullscreen)
	{
		glfwWindowHint(GLFW_RED_BITS, videoMode.redBits);
		glfwWindowHint(GLFW_GREEN_BITS, videoMode.greenBits);
		glfwWindowHint(GLFW_BLUE_BITS, videoMode.blueBits);
		const int refresh_rate_hertz = _refreshRateHertz > 0 ? _refreshRateHertz : videoMode.refreshRate;
		glfwWindowHint(GLFW_REFRESH_RATE, refresh_rate_hertz);
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
