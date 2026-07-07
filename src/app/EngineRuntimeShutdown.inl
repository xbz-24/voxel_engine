	/** Runs frames until the window asks to close. */
	void EngineRuntime::RunMainLoop()
	{
		while (Step()) {}
	}

	bool EngineRuntime::ShouldContinue() const noexcept
	{
		return !window_.ShouldClose() && !engine_.IsStopRequested();
	}

	/** Releases runtime resources and reports shutdown. */
	void EngineRuntime::Shutdown()
	{
		editor_controller_.Shutdown();
		if (view_ != nullptr) view_->ReleaseCachedResources();
		view_.reset();
		vulkan_frame_orchestrator_.Release();
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
