#include "EditorGui.h"

#include "Window.h"

#include <GLFW/glfw3.h>
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

namespace ve::editor
{
	/// Initializes ImGui for the engine window.
	bool EditorGui::Initialize(Window& window)
	{
		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		ImGuiIO& io = ImGui::GetIO();
		io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
		ImGui::StyleColorsDark();
		is_initialized_ = ImGui_ImplGlfw_InitForOpenGL(window.GetNativeWindow(), true) &&
			ImGui_ImplOpenGL3_Init("#version 330");
		return is_initialized_;
	}

	/// Starts a new ImGui frame.
	void EditorGui::BeginFrame()
	{
		if (!is_initialized_) return;
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();
	}

	/// Releases ImGui backends and context.
	void EditorGui::Shutdown()
	{
		if (!is_initialized_) return;
		ImGui_ImplOpenGL3_Shutdown();
		ImGui_ImplGlfw_Shutdown();
		ImGui::DestroyContext();
		is_initialized_ = false;
	}

	/// Reports whether ImGui owns current mouse input.
	bool EditorGui::WantsMouseInput() const noexcept
	{
		return is_initialized_ && ImGui::GetIO().WantCaptureMouse;
	}
}
