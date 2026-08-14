#pragma once

#include "RenderApi.h"
#include "WindowCreateInfo.h"
#include "WindowEvents.h"
#include "WindowNativeOwnership.h"
#include "WindowTypes.h"

#include <string>
#include <string_view>
#include <vector>

struct GLFWmonitor;
struct GLFWvidmode;
struct GLFWwindow;

namespace ve::engine
{
	/** Owns the GLFW window and translates native state into engine events. */
	class Window
	{
	public:
		using CursorMode = WindowCursorMode;
		using WindowSize = ve::engine::WindowSize;
		using CursorPosition = WindowCursorPosition;
		using NativeWindowHandle = ve::engine::NativeWindowHandle;
		using CallbackContext = WindowCallbackContext;

		explicit Window(std::string_view title);
		explicit Window(WindowCreateInfo create_info);
		~Window();
		Window(const Window&) = delete;
		Window& operator=(const Window&) = delete;
		Window(Window&&) = delete;
		Window& operator=(Window&&) = delete;

		bool Initialize();
		bool Initialize(ve::rendering::GraphicsApi graphicsApi);
		void MakeGraphicsContextCurrent() noexcept;
		void Shutdown() noexcept;
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
		[[nodiscard]] WindowSize ClientWindowSize() const noexcept;
		[[nodiscard]] CursorPosition CurrentCursorPosition() const noexcept;
		[[nodiscard]] NativeWindowHandle NativeHandle() const noexcept;
		[[nodiscard]] GLFWwindow* GetNativeWindow() const;
		[[nodiscard]] ve::rendering::GraphicsApi GraphicsApi() const noexcept;
		[[nodiscard]] std::vector<const char*> RequiredVulkanInstanceExtensions() const;

		void SetCallbackUserData(void* userData) noexcept;
		[[nodiscard]] static void* GetCallbackUserData(GLFWwindow* window) noexcept;

		template <typename UserData>
		static UserData* GetCallbackUserDataAs(GLFWwindow* window) noexcept
		{
			return static_cast<UserData*>(GetCallbackUserData(window));
		}

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
		void RecordFramebufferResize(int width, int height) noexcept;
		static CallbackContext* GetCallbackContext(GLFWwindow* window) noexcept;

		UniqueGlfwWindow _window;
		bool _ownsGlfwSession = false;
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
		WindowEventMailbox _events;
	};
}
