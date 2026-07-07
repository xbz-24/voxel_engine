	/** Renders editor panels and presents the native window. */
	void EngineRuntime::EndRuntimeFrame()
	{
		if (view_->Api() == ve::rendering::GraphicsApi::OpenGLCompatibility)
		{
			editor_controller_.DrawAndApply(window_, engine_._runtimeSettings);
			editor_controller_.Render();
		}
		window_.Update();
	}
}
