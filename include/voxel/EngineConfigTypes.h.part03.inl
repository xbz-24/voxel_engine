		/** Disables file log output while preserving the configured path string. */
		LogSettings& DisableFileOutput() noexcept;
	};

	/** Window creation settings used by EngineConfig and the command-line demos. */
	struct WindowConfig
	{
		std::string title = "Voxel Engine";
		int width = 1280;
		int height = 720;
		int monitor_index = 0;
		int refresh_rate_hertz = 0;
		bool fullscreen = false;
		bool vsync = false;
		bool resizable = true;
		bool high_dpi_framebuffer = true;
		bool capture_cursor_on_start = false;

		/** Creates a windowed-mode configuration with a title and client size. */
		[[nodiscard]] static WindowConfig Windowed(std::string title, int width = 1280, int height = 720);

		/** Creates a fullscreen configuration using the selected monitor. */
		[[nodiscard]] static WindowConfig Fullscreen(std::string title = "Voxel Engine");

		/** Enables or disables vertical sync for presentation backends that support it. */
		WindowConfig& EnableVSync(bool enabled = true) noexcept;

		/** Selects the display monitor by zero-based index. */
		WindowConfig& UseMonitor(int index) noexcept;

		/** Requests a fullscreen refresh rate; zero keeps the platform default. */
		WindowConfig& WithRefreshRate(int hertz) noexcept;

		/** Controls whether users can resize the created window. */
		WindowConfig& SetResizable(bool enabled = true) noexcept;

		/** Controls high-DPI framebuffer creation where the platform exposes it. */
		WindowConfig& EnableHighDpiFramebuffer(bool enabled = true) noexcept;

		/** Requests initial mouse capture when the runtime starts. */
		WindowConfig& CaptureCursorOnStart(bool enabled = true) noexcept;
	};

	/** Convenience factory for WindowConfig::Windowed. */
	[[nodiscard]] WindowConfig Window(std::string title, int width = 1280, int height = 720);

	/** Convenience factory for WindowConfig::Fullscreen. */
	[[nodiscard]] WindowConfig Fullscreen(std::string title = "Voxel Engine");
}
