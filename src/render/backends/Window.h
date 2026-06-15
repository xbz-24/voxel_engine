#pragma once

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include "RenderApi.h"
#include <string>
#include <vector>

class Window
{
public:
	/** Cursor modes exposed by the engine window API. */
	enum class CursorMode { Normal, Captured };

	/** Callback data stored inside GLFW user pointer. */
	struct CallbackContext
	{
		Window* window;
		void* userData;
	};

	/// Creates a window wrapper with a title.
	explicit Window(const char* title);
	/// Creates a window wrapper with a title.
	explicit Window(std::string title);
	/// Destroys the native GLFW window and terminates GLFW.
	~Window();
	/// Initializes the window with the default Vulkan backend.
	bool Initialize();
	/** @param graphicsApi Graphics API that controls native window hints and context setup. @return True when ready. */
	bool Initialize(ve::rendering::GraphicsApi graphicsApi);
	/// Enables or disables vertical synchronization for buffer swaps.
	void SetVSync(bool isEnabled);
	/// Returns whether vertical synchronization is enabled.
	bool IsVSyncEnabled() const noexcept;
	/// Applies the cursor mode used by menus and first-person camera control.
	void SetCursorMode(CursorMode mode);
	/// Swaps buffers and polls window events.
	void Update();
	/// Checks whether GLFW requested the window to close.
	bool ShouldClose() const;
	/// Requests that the window closes.
	void Close();
	/// Returns the current framebuffer width.
	int GetWidth() const;
	/// Returns the current framebuffer height, clamped to at least 1.
	int GetHeight() const;
	/// Returns the current framebuffer aspect ratio.
	float GetAspectRatio() const;
	/// Returns the native GLFW window pointer.
	GLFWwindow* GetNativeWindow() const;
	/** @return Graphics API used to initialize this window. */
	/// Returns the graphics API used to initialize this window.
	[[nodiscard]] ve::rendering::GraphicsApi GraphicsApi() const noexcept;
	/// Returns required Vulkan instance extensions reported by GLFW.
	[[nodiscard]] std::vector<const char*> RequiredVulkanInstanceExtensions() const;
	/// Stores user data reachable from GLFW callbacks.
	void SetCallbackUserData(void* userData);
	/// Reads user data from a native GLFW window callback context.
	static void* GetCallbackUserData(GLFWwindow* window);
	/// Updates stored framebuffer dimensions after a GLFW resize event.
	static void FramebufferResizeCallback(GLFWwindow* window, int width, int height) noexcept;

private:
	/// Starts GLFW and reports whether initialization succeeded.
	bool InitializeGlfw();
	/// Reads the primary monitor video mode and updates window dimensions.
	const GLFWvidmode* ReadPrimaryMonitorMode();
	/// Applies GLFW window hints from a video mode.
	void ApplyWindowHints(const GLFWvidmode& videoMode);
	/// Applies graphics API specific GLFW hints.
	void ApplyGraphicsApiHints();
	/// Creates the native GLFW window.
	bool CreateNativeWindow();
	/// Wires GLFW user data, callbacks and current context.
	void ConfigureNativeCallbacks();
	/// Initializes GLEW after the OpenGL context exists.
	bool InitializeOpenGLLoader();

	GLFWwindow* _window;
	int _width;
	int _height;
	bool _isVSyncEnabled;
	ve::rendering::GraphicsApi _graphicsApi;
	std::string _title;
	CallbackContext _callbackContext;
};
