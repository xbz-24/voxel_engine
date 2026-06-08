#pragma once

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <GL/glew.h>
#include <string>

class Window
{
public:
	/**
	 * Cursor modes exposed by the engine window API.
	 */
	enum class CursorMode
	{
		Normal,
		Captured
	};

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

	/// Initializes GLFW, creates the native window, and initializes GLEW.
	bool Initialize();

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

	/// Stores user data reachable from GLFW callbacks.
	void SetCallbackUserData(void* userData);

	/// Reads user data from a native GLFW window callback context.
	static void* GetCallbackUserData(GLFWwindow* window);

	/// Updates stored framebuffer dimensions after a GLFW resize event.
	static void FramebufferResizeCallback(GLFWwindow* window, int width, int height);

private:
	/// Starts GLFW and reports whether initialization succeeded.
	bool InitializeGlfw();
	/// Reads the primary monitor video mode and updates window dimensions.
	const GLFWvidmode* ReadPrimaryMonitorMode();
	/// Applies GLFW window hints from a video mode.
	void ApplyWindowHints(const GLFWvidmode& videoMode);
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
	std::string _title;
	CallbackContext _callbackContext;
};
