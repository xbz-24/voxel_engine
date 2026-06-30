namespace voxel
{
	CameraConfig CameraConfig::At(Vec3 position, Vec3 look_at) noexcept
	{
		return CameraConfig{ position, look_at, true };
	}

	CameraConfig Camera(Vec3 position, Vec3 look_at) noexcept
	{
		return CameraConfig::At(position, look_at);
	}

	WindowConfig WindowConfig::Windowed(std::string title, int width, int height)
	{
		WindowConfig config{};
		config.title = std::move(title);
		config.width = width;
		config.height = height;
		config.fullscreen = false;
		return config;
	}

	WindowConfig WindowConfig::Fullscreen(std::string title)
	{
		WindowConfig config{};
		config.title = std::move(title);
		config.fullscreen = true;
		return config;
	}

	WindowConfig& WindowConfig::EnableVSync(bool enabled) noexcept
	{
		vsync = enabled;
		return *this;
	}

	WindowConfig& WindowConfig::UseMonitor(int index) noexcept
	{
		monitor_index = index;
		return *this;
	}

	WindowConfig& WindowConfig::WithRefreshRate(int hertz) noexcept
	{
		refresh_rate_hertz = hertz;
		return *this;
	}

	WindowConfig& WindowConfig::SetResizable(bool enabled) noexcept
	{
		resizable = enabled;
		return *this;
	}

	WindowConfig& WindowConfig::EnableHighDpiFramebuffer(bool enabled) noexcept
	{
		high_dpi_framebuffer = enabled;
		return *this;
	}

	WindowConfig& WindowConfig::CaptureCursorOnStart(bool enabled) noexcept
	{
		capture_cursor_on_start = enabled;
		return *this;
	}

	WindowConfig Window(std::string title, int width, int height)
	{
		return WindowConfig::Windowed(std::move(title), width, height);
	}

	WindowConfig Fullscreen(std::string title)
	{
		return WindowConfig::Fullscreen(std::move(title));
	}
