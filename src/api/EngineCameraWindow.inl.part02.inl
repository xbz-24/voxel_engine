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
