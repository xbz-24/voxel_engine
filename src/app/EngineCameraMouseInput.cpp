#include "VoxelSandboxModule.h"

#include "RuntimeInputRouter.h"
#include "Window.h"

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

void ve::engine::VoxelSandboxModule::ConfigureInputCallbacks()
{
	window_->SetCallbackUserData(&input_router_);
	glfwSetCursorPosCallback(window_->GetNativeWindow(), RuntimeInputRouter::CursorPositionCallback);
	window_->SetCursorMode(Window::CursorMode::Captured);
}
