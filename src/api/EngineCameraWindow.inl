namespace voxel
{
	LogSettings& LogSettings::WithMinimumLevel(LogLevel level) noexcept
	{
		minimum_level = level;
		return *this;
	}

	LogSettings& LogSettings::EnableConsole(bool enabled) noexcept
	{
		console_enabled = enabled;
		return *this;
	}

	LogSettings& LogSettings::DisableConsole() noexcept
	{
		return EnableConsole(false);
	}

	LogSettings& LogSettings::WriteToFile(std::string path)
	{
		file_output_enabled = true;
		file_output_path = std::move(path);
		return *this;
	}

	LogSettings& LogSettings::DisableFileOutput() noexcept
	{
		file_output_enabled = false;
		file_output_path.clear();
		return *this;
	}

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
