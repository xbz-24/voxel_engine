#include "Engine.h"

#include "Logger.h"
#include "Window.h"

void Engine::ConfigureCallbacks(ve::engine::Window& window, CallbackContext& context)
{
	window.SetCallbackUserData(&context);
	glfwSetCursorPosCallback(window.GetNativeWindow(), mouse_callback);
	window.SetCursorMode(ve::engine::Window::CursorMode::Captured);
}

void Engine::mouse_callback(GLFWwindow* window, double currentMouseCursorPosX, double currentMouseCursorPosY) noexcept
{
	CallbackContext* context = static_cast<CallbackContext*>(ve::engine::Window::GetCallbackUserData(window));
	if (!context || !context->camera)
	{
		ve::log::Error("Camera pointer is null in mouse callback");
		return;
	}
	if (context->isSettingsMenuOpen && *context->isSettingsMenuOpen)
	{
		context->mouse.isFirstInputEvent = true;
		return;
	}

	if (context->mouse.isFirstInputEvent)
	{
		context->mouse.previousX = currentMouseCursorPosX;
		context->mouse.previousY = currentMouseCursorPosY;
		context->mouse.isFirstInputEvent = false;
	}

	const double deltaX = currentMouseCursorPosX - context->mouse.previousX;
	const double deltaY = context->mouse.previousY - currentMouseCursorPosY;
	context->mouse.previousX = currentMouseCursorPosX;
	context->mouse.previousY = currentMouseCursorPosY;

	constexpr float mouseLookSensitivity = 0.1f;
	context->camera->Yaw(static_cast<float>(deltaX) * mouseLookSensitivity);
	context->camera->Pitch(static_cast<float>(deltaY) * mouseLookSensitivity);
}
