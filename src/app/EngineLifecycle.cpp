#include "Engine.h"

#include "Logger.h"
#include "RenderBackendSelector.h"

#include <algorithm>

/// Initializes the native window and applies runtime window options.
bool EngineApplication::InitializeWindow(ve::engine::Window& window)
{
	// TODO: Return structured startup errors instead of a bool so the public API can report missing drivers/backends clearly.
	const ve::rendering::GraphicsApi graphics_api = ve::rendering::RenderBackendSelector::SelectApi(
		_runtimeSettings.renderBackendConfiguration);
	VE_LOG_CATEGORY_INFO(ve::log::category::Engine, ve::rendering::RenderBackendSelector::Name(graphics_api));
	if (!window.Initialize(graphics_api))
	{
		return false;
	}
	window.SetVSync(_runtimeSettings.isVSyncEnabled);
	return true;
}

/// Initializes logger outputs that need the resolved project root.
void EngineApplication::ConfigureRuntimeLogging(const ve::assets::AssetPaths& assetPaths)
{
	// TODO: Let public EngineConfig override log sinks before the runtime decides the default file path.
	constexpr ve::log::Level minimum_level =
#if defined(NDEBUG)
		ve::log::Level::Info;
#else
		ve::log::Level::Debug;
#endif
	ve::log::ApplyConfiguration(ve::log::LoggerConfiguration{
		minimum_level,
		true,
		assetPaths.rootDirectory / "logs/engine.log"
	});
	VE_LOG_CATEGORY_INFO(ve::log::category::Engine, "Engine runtime started");
}

/// Updates projection matrices from resize events queued by the window backend.
void EngineApplication::UpdateProjectionIfWindowChanged(ve::engine::Window& window)
{
	for (const ve::engine::WindowEvent& event : window.DrainEvents())
	{
		if (event.kind == ve::engine::WindowEvent::Kind::FramebufferResized)
		{
			ApplyFramebufferSize(event.framebuffer_resized.width, event.framebuffer_resized.height);
		}
	}

	if (_window_state.current_width == 0 || _window_state.current_height == 0)
	{
		ApplyFramebufferSize(window.GetWidth(), window.GetHeight());
	}
}

/// Updates projection matrices only when the framebuffer dimensions changed.
void EngineApplication::ApplyFramebufferSize(int width, int height)
{
	const int clamped_width = std::max(1, width);
	const int clamped_height = std::max(1, height);
	if (clamped_width == _window_state.current_width && clamped_height == _window_state.current_height)
	{
		return;
	}
	_window_state.current_width = clamped_width;
	_window_state.current_height = clamped_height;
	UpdateProjections(_window_state.current_width, _window_state.current_height);
}

/// Releases cached OpenGL resources owned directly by the engine.
void EngineApplication::ReleaseRenderCaches()
{
	// TODO: Move remaining direct OpenGL cache ownership into OpenGLRenderView.
	if (_render_cache_state.cloud_display_list_id == 0)
	{
		return;
	}
	glDeleteLists(_render_cache_state.cloud_display_list_id, 1);
	_render_cache_state.cloud_display_list_id = 0;
}
