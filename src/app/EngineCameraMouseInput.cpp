#include "Engine.h"

#include "Logger.h"
#include "Window.h"

void EngineApplication::ConfigureCallbacks(ve::engine::Window& window, CallbackContext& context)
{
	// TODO: Route callbacks through an input router so ImGui, public callbacks, and gameplay share capture rules.
	context.mouse_look_settings = &_runtimeSettings.mouse_look;
	window.SetCallbackUserData(&context);
	glfwSetCursorPosCallback(window.GetNativeWindow(), mouse_callback);
	window.SetCursorMode(ve::engine::Window::CursorMode::Captured);
}

void EngineApplication::mouse_callback(GLFWwindow* window, double currentMouseCursorPosX, double currentMouseCursorPosY) noexcept
{
	CallbackContext* context = static_cast<CallbackContext*>(ve::engine::Window::GetCallbackUserData(window));
	if (!context || !context->camera)
	{
		ve::log::Error("Camera pointer is null in mouse callback");
		return;
	}
	if (context->isSettingsMenuOpen && *context->isSettingsMenuOpen)
	{
		ve::engine::ResetMouseLookState(context->mouse_look);
		return;
	}

	const ve::gameplay::MouseLookSettings mouse_look_settings =
		context->mouse_look_settings != nullptr ? *context->mouse_look_settings : ve::gameplay::MouseLookSettings{};
	const std::optional<ve::engine::MouseLookDelta> mouse_look_delta =
		ve::engine::ConsumeMouseLookDelta(
			context->mouse_look,
			currentMouseCursorPosX,
			currentMouseCursorPosY,
			mouse_look_settings);
	if (!mouse_look_delta.has_value())
	{
		return;
	}

	context->camera->Yaw(mouse_look_delta->yaw_degrees);
	context->camera->Pitch(mouse_look_delta->pitch_degrees);
}
