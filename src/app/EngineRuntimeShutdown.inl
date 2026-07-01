	/** Returns the OpenGL compatibility view after startup validation. */
	OpenGLRenderView& EngineRuntime::LegacyOpenGLView() noexcept
	{
		OpenGLRenderView* legacy_view = view_ ? TryRenderViewCast<OpenGLRenderView>(*view_) : nullptr;
		assert(legacy_view != nullptr);
		return *legacy_view;
	}

	/** Runs frames until the window asks to close. */
	void EngineRuntime::RunMainLoop()
	{
		while (true)
		{
			if (window_.ShouldClose() || engine_.IsStopRequested()) break;
			RunFrame();
		}
	}

	/** Releases runtime resources and reports shutdown. */
	void EngineRuntime::Shutdown()
	{
		editor_controller_.Shutdown();
		if (window_.GraphicsApi() == ve::rendering::GraphicsApi::OpenGLCompatibility) engine_.ReleaseRenderCaches();
		view_.reset();
		vulkan_frame_renderer_.Release();
		if (backend_ == nullptr)
		{
			VE_LOG_CATEGORY_WARNING(ve::log::category::Engine, "No render backend to release");
			model_.reset();
			VE_LOG_CATEGORY_INFO(ve::log::category::Engine, "Engine runtime stopped");
			return;
		}
		if (backend_->Api() == ve::rendering::GraphicsApi::Vulkan)
		{
			static_cast<ve::rendering::VulkanBackend&>(*backend_).Release();
		}
		backend_.reset();
		model_.reset();
		VE_LOG_CATEGORY_INFO(ve::log::category::Engine, "Engine runtime stopped");
	}

}
