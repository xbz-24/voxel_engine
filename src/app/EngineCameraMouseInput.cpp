#include "Engine.h"

#include "RuntimeInputRouter.h"
#include "Window.h"

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

void EngineApplication::ConfigureCallbacks(ve::engine::Window& window, ve::engine::RuntimeInputRouter& input_router)
{
	window.SetCallbackUserData(&input_router);
	glfwSetCursorPosCallback(window.GetNativeWindow(), ve::engine::RuntimeInputRouter::CursorPositionCallback);
	window.SetCursorMode(ve::engine::Window::CursorMode::Captured);
}
