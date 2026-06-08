#pragma once

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <string>
#include <GL/glew.h>
class Window
{
public:
	struct CallbackContext
	{
		Window* window;
		void* userData;
	};

	/**
	 * Creates a window wrapper with a title.
	 *
	 * @param title Window title.
	 */
	explicit Window(const char* title);

	/**
	 * Creates a window wrapper with a title.
	 *
	 * @param title Window title.
	 */
	explicit Window(std::string title);

	/**
	 * Destroys the native GLFW window and terminates GLFW.
	 */
	~Window();

	/**
	 * Initializes GLFW, creates the native window, and initializes GLEW.
	 *
	 * @return true when initialization succeeds, false otherwise.
	 */
	bool Initialize();

	/**
	 * Enables or disables vertical synchronization for buffer swaps.
	 *
	 * @param isEnabled true to wait for monitor refresh, false to uncap swaps.
	 */
	void SetVSync(bool isEnabled);

	/**
	 * Returns whether vertical synchronization is enabled.
	 *
	 * @return true when swaps are synchronized to monitor refresh.
	 */
	bool IsVSyncEnabled() const noexcept;

	/**
	 * Swaps buffers and polls window events.
	 */
	void Update();

	/**
	 * Checks whether GLFW requested the window to close.
	 *
	 * @return true when the window should close.
	 */
	bool ShouldClose() const;

	/**
	 * Requests that the window closes.
	 */
	void Close();

	/**
	 * Returns the current framebuffer width.
	 *
	 * @return Width in pixels.
	 */
	int GetWidth() const;

	/**
	 * Returns the current framebuffer height.
	 *
	 * @return Height in pixels, clamped to at least 1.
	 */
	int GetHeight() const;

	/**
	 * Returns the current framebuffer aspect ratio.
	 *
	 * @return Width divided by height.
	 */
	float GetAspectRatio() const;

	/**
	 * Returns the native GLFW window pointer.
	 *
	 * @return Native GLFWwindow pointer.
	 */
	GLFWwindow* GetNativeWindow() const;

	/**
	 * Stores user data reachable from GLFW callbacks.
	 *
	 * @param userData Opaque pointer owned by the caller.
	 */
	void SetCallbackUserData(void* userData);

	/**
	 * Reads user data from a native GLFW window callback context.
	 *
	 * @param window Native GLFW window carrying a CallbackContext.
	 * @return Opaque user data pointer, or nullptr when unavailable.
	 */
	static void* GetCallbackUserData(GLFWwindow* window);

	/**
	 * Updates stored framebuffer dimensions after a GLFW resize event.
	 *
	 * @param window Native GLFW window carrying this Window as user pointer.
	 * @param width New framebuffer width.
	 * @param height New framebuffer height.
	 */
	static void FramebufferResizeCallback(GLFWwindow* window, int width, int height);
private:
	GLFWwindow* _window;
	int _width;
	int _height;
	bool _isVSyncEnabled;
	std::string _title;
	CallbackContext _callbackContext;
};

