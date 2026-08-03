		/** Requests the native window to close on the next update. */
		void Close();

		/** @return Current framebuffer width in pixels. */
		[[nodiscard]] int GetWidth() const;

		/** @return Current framebuffer height in pixels. */
		[[nodiscard]] int GetHeight() const;

		/** @return Current framebuffer width divided by height. */
		[[nodiscard]] float GetAspectRatio() const;

		/** @return Current client framebuffer size. */
		[[nodiscard]] WindowSize ClientWindowSize() const noexcept;

		/** @return Last cursor position reported by GLFW. */
		[[nodiscard]] CursorPosition CurrentCursorPosition() const noexcept;

		/** @return Opaque handle to the native GLFW window. */
		[[nodiscard]] NativeWindowHandle NativeHandle() const noexcept;

		/** @return Native GLFW window pointer for Vulkan surface creation and callbacks. */
		[[nodiscard]] GLFWwindow* GetNativeWindow() const;

		/** @return Graphics API this window was configured to support. */
		[[nodiscard]] ve::rendering::GraphicsApi GraphicsApi() const noexcept;

		/** @return Vulkan instance extensions required by the native window system. */
		[[nodiscard]] std::vector<const char*> RequiredVulkanInstanceExtensions() const;

		/** Stores user data reachable from GLFW callbacks. */
		void SetCallbackUserData(void* userData);

		/** @return User data previously stored on the native window callback context. */
		static void* GetCallbackUserData(GLFWwindow* window);

		/** @return Typed user data previously stored on the native window callback context. */
		template <typename UserData>
		static UserData* GetCallbackUserDataAs(GLFWwindow* window)
		{
			return static_cast<UserData*>(GetCallbackUserData(window));
		}

		/** Records resize events from the native framebuffer callback. */
		static void FramebufferResizeCallback(GLFWwindow* window, int width, int height) noexcept;

	private:
		struct GlfwWindowDeleter
		{
			void operator()(GLFWwindow* window) const noexcept;
		};

		bool InitializeGlfw();
		GLFWmonitor* SelectDisplayMonitor();
		const GLFWvidmode* ReadDisplayMode(GLFWmonitor* display_monitor);
		void ApplyWindowHints(const GLFWvidmode& videoMode);
		void ApplyGraphicsApiHints();
		bool CreateNativeWindow(GLFWmonitor* fullscreen_monitor);
		void ApplyInitialCursorMode();
		void ConfigureNativeCallbacks();
		void RecordFramebufferResize(int width, int height);
		static CallbackContext* GetCallbackContext(GLFWwindow* window);

		std::unique_ptr<GLFWwindow, GlfwWindowDeleter> _window;
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
