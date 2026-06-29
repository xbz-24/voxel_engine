	EngineConfig& EngineConfig::UseGraphicsApi(GraphicsApi value) noexcept
	{
		graphics_api = value;
		return *this;
	}

	EngineConfig& EngineConfig::UseVulkan() noexcept
	{
		return UseGraphicsApi(GraphicsApi::Vulkan);
	}

	EngineConfig& EngineConfig::UseOpenGLCompatibility() noexcept
	{
		return UseGraphicsApi(GraphicsApi::OpenGLCompatibility);
	}

	EngineConfig& EngineConfig::UseDirectX12() noexcept
	{
		return UseGraphicsApi(GraphicsApi::DirectX12);
	}

	EngineConfig& EngineConfig::WithDemoScene(DemoScene value) noexcept
	{
		demo_scene = value;
		return *this;
	}

	EngineConfig& EngineConfig::WithWorldSizeChunks(int value) noexcept
	{
		world.size_chunks = value;
		return *this;
	}

	EngineConfig& EngineConfig::WithRenderDistanceChunks(int value) noexcept
	{
		render_distance_chunks = value;
		return *this;
	}

	EngineConfig& EngineConfig::ShowDebugOverlay(bool enabled) noexcept
	{
		show_debug_overlay = enabled;
		return *this;
	}

	EngineConfig& EngineConfig::HideDebugOverlay() noexcept
	{
		return ShowDebugOverlay(false);
	}

	EngineConfig& EngineConfig::OnUpdate(UpdateCallback callback) noexcept
	{
		on_update = std::move(callback);
		return *this;
	}

	EngineConfig& EngineConfig::OnDiagnostics(DiagnosticsCallback callback) noexcept
	{
		on_diagnostics = std::move(callback);
		return *this;
	}

	EngineConfig& EngineConfig::OnLog(LogCallback callback) noexcept
	{
		on_log = std::move(callback);
		return *this;
	}
