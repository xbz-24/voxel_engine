#include "Engine.h"

#include "Logger.h"
#include "RenderBackendSelector.h"

/// Initializes the native window and applies runtime window options.
bool Engine::InitializeWindow(Window& window)
{
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
void Engine::ConfigureRuntimeLogging(const ve::assets::AssetPaths& assetPaths)
{
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

/// Updates projection matrices only when the window dimensions changed.
void Engine::UpdateProjectionIfWindowChanged(const Window& window)
{
	if (window.GetWidth() == _window_state.current_width && window.GetHeight() == _window_state.current_height)
	{
		return;
	}
	_window_state.current_width = window.GetWidth();
	_window_state.current_height = window.GetHeight();
	UpdateProjections(_window_state.current_width, _window_state.current_height);
}

/// Releases cached OpenGL resources owned directly by the engine.
void Engine::ReleaseRenderCaches()
{
	if (_render_cache_state.cloud_display_list_id == 0)
	{
		return;
	}
	glDeleteLists(_render_cache_state.cloud_display_list_id, 1);
	_render_cache_state.cloud_display_list_id = 0;
}
