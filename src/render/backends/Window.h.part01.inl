
#include "RenderApi.h"
#include "WindowCreateInfo.h"
#include "WindowEvents.h"

#include <memory>
#include <string>
#include <string_view>
#include <vector>

struct GLFWmonitor;
struct GLFWvidmode;
struct GLFWwindow;

namespace ve::engine
{
	/** Owns the GLFW window and translates native window state into engine events. */
	class Window
	{
	public:
		/** Mouse cursor ownership mode used by interactive demos. */
		enum class CursorMode { Normal, Captured };

		/** Pixel size of the client framebuffer. */
		struct WindowSize
		{
			int width = 1;
			int height = 1;
		};

		/** Last known cursor position in window coordinates. */
		struct CursorPosition
		{
			double x = 0.0;
			double y = 0.0;
		};

		/** Opaque native window handle for backend-specific integrations. */
		struct NativeWindowHandle
		{
			void* opaque_handle = nullptr;
		};

		/** User data stored on GLFW callbacks so static callbacks can reach this Window. */
		struct CallbackContext
		{
			Window* window;
			void* userData;
		};

		/** Creates a default window with the supplied title. */
		explicit Window(std::string_view title);

		/** Creates a window from explicit startup settings. */
		explicit Window(WindowCreateInfo create_info);

		/** Releases the native window and associated GLFW state. */
		~Window();

		Window(const Window&) = delete;
		Window& operator=(const Window&) = delete;
		Window(Window&&) = delete;
		Window& operator=(Window&&) = delete;

		/** Initializes the native window using the configured graphics API. */
		bool Initialize();

		/** Initializes the native window for the requested graphics API. */
		bool Initialize(ve::rendering::GraphicsApi graphicsApi);

		/** Enables or disables swap interval presentation after initialization. */
		void SetVSync(bool isEnabled);

		/** @return True when swap interval presentation is enabled. */
		[[nodiscard]] bool IsVSyncEnabled() const noexcept;

		/** Applies a mouse cursor capture mode to the native window. */
		void SetCursorMode(CursorMode mode);

		/** Polls native events and updates cached window state. */
		void Update();

		/** @return Pending engine window events, clearing the internal queue. */
		[[nodiscard]] std::vector<WindowEvent> DrainEvents();

		/** @return True when the native window requested shutdown. */
		[[nodiscard]] bool ShouldClose() const;

