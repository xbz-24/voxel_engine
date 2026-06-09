#include "Engine.h"

#include "AssetPaths.h"
#include "GameController.h"
#include "GameModel.h"
#include "GameView.h"
#include "Logger.h"

#include <memory>

namespace
{
	constexpr int DefaultWorldSizeChunks = 10;
}

struct Engine::RuntimeContext
{
	Window window{ "Voxel Engine v1.0.0" };
	CallbackContext callbackContext{ nullptr, nullptr, { 0.0, 0.0, true } };
	ve::assets::AssetPaths assetPaths;
	std::unique_ptr<ve::engine::GameModel> model;
	std::unique_ptr<ve::engine::GameView> view;
	ve::engine::GameController controller;
	ve::time::FrameTimer frameTimer;
};

/// Starts the engine and returns a process-style status code.
int Engine::Run()
{
	std::unique_ptr<RuntimeContext> runtime = CreateRuntimeContext();
	if (!runtime)
	{
		return -1;
	}

	RunMainLoop(*runtime);
	ReleaseRenderCaches();
	VE_LOG_CATEGORY_INFO(ve::log::category::Engine, "Engine runtime stopped");
	return 0;
}

/// Builds all runtime systems needed by the main loop.
std::unique_ptr<Engine::RuntimeContext> Engine::CreateRuntimeContext()
{
	auto runtime = std::make_unique<RuntimeContext>();
	runtime->callbackContext.isSettingsMenuOpen = &_runtimeSettings.isSettingsMenuOpen;
	if (!InitializeWindow(runtime->window))
	{
		return nullptr;
	}

	ConfigureOpenGLState();
	runtime->assetPaths = ve::assets::ResolveFromSourceFile(_applicationSourceFilePath);
	ConfigureRuntimeLogging(runtime->assetPaths);
	runtime->model = std::make_unique<ve::engine::GameModel>(DefaultWorldSizeChunks);
	runtime->callbackContext.camera = &runtime->model->MutableCamera();
	ConfigureCallbacks(runtime->window, runtime->callbackContext);
	LoadRuntimeAssets(*runtime);
	return runtime;
}

/// Loads OpenGL-backed assets after a window context exists.
void Engine::LoadRuntimeAssets(RuntimeContext& runtime)
{
	runtime.view = std::make_unique<ve::engine::GameView>(runtime.assetPaths);
}

/// Runs the frame loop until the active window closes.
void Engine::RunMainLoop(RuntimeContext& runtime)
{
	while (!runtime.window.ShouldClose())
	{
		RunFrame(runtime);
	}
}

/// Updates gameplay, renders, and presents one frame.
void Engine::RunFrame(RuntimeContext& runtime)
{
	UpdateProjectionIfWindowChanged(runtime.window);
	runtime.frameTimer.Tick();
	runtime.controller.Update(*this, runtime.window, *runtime.model, runtime.view->MutableBlockRegistry(), runtime.frameTimer.DeltaSeconds());
	Render3DWorld(runtime.window, runtime.model->MutableCamera(), runtime.view->MutableSkyBox(), runtime.view->MutablePlane(), runtime.view->MutableSelectionCube(), runtime.view->MutableBlockRegistry(), runtime.model->MutableWorld(), runtime.model->GetSelection());
	runtime.view->MutableHudRenderer().Draw(CreateHudFrame(runtime.window, runtime.model->GetCamera(), runtime.frameTimer, runtime.model->GetSelection(), runtime.view->MutableBlockRegistry(), runtime.model->GetWorld()));
	runtime.window.Update();
}
