#include "EditorGui.h"

#include "Window.h"

#include <GLFW/glfw3.h>
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

namespace ve::editor
{
	/**
	* @note Enforces a strict hardware API check before initializing. This prevents fatal
	* access violations if the engine is booted in a different graphics mode (e.g., Vulkan),
	* as the ImGui_ImplOpenGL3 backend assumes a valid GL context is already current.
	*/

	bool EditorGui::Initialize(Window& window)
	{
		if (window.GraphicsApi() != ve::rendering::GraphicsApi::OpenGLCompatibility) return false;
		IMGUI_CHECKVERSION();
			ImGui::CreateContext();
			ImGuiIO& io = ImGui::GetIO();
			io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
			ImGui::StyleColorsDark();
			is_initialized_ = ImGui_ImplGlfw_InitForOpenGL(window.GetNativeWindow(), true) &&
				ImGui_ImplOpenGL3_Init("#version 330");
			return is_initialized_;
	}

    /**
    * @note The execution order here is strictly mandated by Dear ImGui's backend architecture.
    * The graphics backend (OpenGL3) must prepare render state, followed by the platform backend
    * (GLFW) updating inputs and delta time, before the core ImGui context is finally advanced.
    */

	void EditorGui::BeginFrame()
	{
		if (!is_initialized_) return;
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();
	}

	/**
	* @note Destruction strictly occurs in the reverse order of initialization to
	* prevent use-after-free conditions in the backends.
	*/ 

	void EditorGui::Shutdown()
	{
		if (!is_initialized_) return;
		ImGui_ImplOpenGL3_Shutdown();
		ImGui_ImplGlfw_Shutdown();
		ImGui::DestroyContext();
		is_initialized_ = false;
	}

	/**
	* @note Short-circuit evaluation guards against querying the ImGui IO structure when the
	* context is dormant or destroyed. Relies on ImGui's internal `WantCaptureMouse` flag to
	* seamlessly dictate when the core engine should suppress raycasting or camera logic.
	*/

	bool EditorGui::WantsMouseInput() const noexcept
	{
		return is_initialized_ && ImGui::GetIO().WantCaptureMouse;
	}
}
