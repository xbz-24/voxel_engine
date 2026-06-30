#pragma once

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include "RenderApi.h"

#include <string>
#include <string_view>
#include <vector>

namespace ve::engine
{
	struct WindowFramebufferResizeEvent
	{
		int width = 0;
		int height = 0;
	};

	struct WindowEvent
	{
		enum class Kind
		{
			FramebufferResized
		};

		Kind kind = Kind::FramebufferResized;
		WindowFramebufferResizeEvent framebuffer_resized{};
	};

	struct WindowCreateInfo
	{
		std::string title = "Voxel Engine v1.0.0";
		int width = 1280;
		int height = 720;
		int display_index = 0;
		int refresh_rate_hertz = 0;
		bool fullscreen = false;
		bool resizable = true;
		bool high_dpi_framebuffer = true;
		bool capture_cursor_on_start = false;
	};

	class Window
	{
	public:
		// TODO: Abstract GLFW behind an input/window backend before exposing native handles in the public SDK.
		enum class CursorMode { Normal, Captured };

		struct NativeWindowHandle
		{
			void* glfwWindow = nullptr;
		};

		struct CallbackContext
		{
			Window* window;
			void* userData;
		};

		explicit Window(std::string_view title);
		explicit Window(WindowCreateInfo create_info);
		~Window();

		Window(const Window&) = delete;
		Window& operator=(const Window&) = delete;
		Window(Window&&) = delete;
		Window& operator=(Window&&) = delete;

		bool Initialize();
		bool Initialize(ve::rendering::GraphicsApi graphicsApi);

		void SetVSync(bool isEnabled);
		[[nodiscard]] bool IsVSyncEnabled() const noexcept;

		void SetCursorMode(CursorMode mode);
		void Update();
		[[nodiscard]] std::vector<WindowEvent> DrainEvents();

		[[nodiscard]] bool ShouldClose() const;
		void Close();

		[[nodiscard]] int GetWidth() const;
		[[nodiscard]] int GetHeight() const;
		[[nodiscard]] float GetAspectRatio() const;
		[[nodiscard]] NativeWindowHandle NativeHandle() const noexcept;
		[[nodiscard]] GLFWwindow* GetNativeWindow() const;

		[[nodiscard]] ve::rendering::GraphicsApi GraphicsApi() const noexcept;
		[[nodiscard]] std::vector<const char*> RequiredVulkanInstanceExtensions() const;

		void SetCallbackUserData(void* userData);
		static void* GetCallbackUserData(GLFWwindow* window);
		static void FramebufferResizeCallback(GLFWwindow* window, int width, int height) noexcept;

	private:
		bool InitializeGlfw();
		GLFWmonitor* SelectDisplayMonitor();
		const GLFWvidmode* ReadDisplayMode(GLFWmonitor* display_monitor);
		void ApplyWindowHints(const GLFWvidmode& videoMode);
		void ApplyGraphicsApiHints();
		bool CreateNativeWindow(GLFWmonitor* fullscreen_monitor);
		void ApplyInitialCursorMode();
		void ConfigureNativeCallbacks();
		void RecordFramebufferResize(int width, int height);

		GLFWwindow* _window = nullptr;
		int _width = 0;
		int _height = 0;
		int _displayIndex = 0;
		int _refreshRateHertz = 0;
		bool _isVSyncEnabled = false;
		ve::rendering::GraphicsApi _graphicsApi = ve::rendering::GraphicsApi::Vulkan;
		std::string _title;
		bool _fullscreen = false;
		bool _resizable = true;
		bool _highDpiFramebuffer = true;
		bool _captureCursorOnStart = false;
		CallbackContext _callbackContext{};
		std::vector<WindowEvent> _eventQueue;
	};
}
