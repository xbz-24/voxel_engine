#include "Engine.h"

#include "Logger.h"

/// Initializes the native window and applies runtime window options.
bool Engine::InitializeWindow(Window& window)
{
	if (!window.Initialize())
	{
		return false;
	}
	window.SetVSync(_runtimeSettings.isVSyncEnabled);
	return true;
}

/// Initializes logger outputs that need the resolved project root.
void Engine::ConfigureRuntimeLogging(const ve::assets::AssetPaths& assetPaths)
{
	ve::log::SetFileOutput(assetPaths.rootDirectory / "logs/engine.log");
	VE_LOG_INFO("Engine runtime started");
}

/// Updates projection matrices only when the window dimensions changed.
void Engine::UpdateProjectionIfWindowChanged(const Window& window)
{
	if (window.GetWidth() == _currentWindowWidth && window.GetHeight() == _currentWindowHeight)
	{
		return;
	}
	_currentWindowWidth = window.GetWidth();
	_currentWindowHeight = window.GetHeight();
	UpdateProjections(_currentWindowWidth, _currentWindowHeight);
}

/// Releases cached OpenGL resources owned directly by the engine.
void Engine::ReleaseRenderCaches()
{
	if (_cloudDisplayListID == 0)
	{
		return;
	}
	glDeleteLists(_cloudDisplayListID, 1);
	_cloudDisplayListID = 0;
}
