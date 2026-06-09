#include "Engine.h"

#include "AssetPaths.h"
#include "Logger.h"

#include <memory>

namespace
{
	constexpr int DefaultWorldSizeChunks = 10;
}

struct Engine::RuntimeContext
{
	Window window{ "Voxel Engine v1.0.0" };
	Camera camera;
	CallbackContext callbackContext{ &camera, nullptr, { 0.0, 0.0, true } };
	ve::assets::AssetPaths assetPaths;
	std::unique_ptr<SkyBox> skyBox;
	std::unique_ptr<BlockSelectionCube> selectionCube;
	std::unique_ptr<Plane> plane;
	std::unique_ptr<ve::blocks::BlockRegistry> blockRegistry;
	std::unique_ptr<ve::world::World> world;
	std::unique_ptr<ve::ui::HudRenderer> hudRenderer;
	ve::time::FrameTimer frameTimer;
	BlockSelection currentSelection{ false, glm::ivec3(0), glm::ivec3(0) };
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

	ConfigureCallbacks(runtime->window, runtime->callbackContext);
	ConfigureOpenGLState();
	runtime->assetPaths = ve::assets::ResolveFromSourceFile(_applicationSourceFilePath);
	ConfigureRuntimeLogging(runtime->assetPaths);
	LoadRuntimeAssets(*runtime);
	return runtime;
}

/// Loads OpenGL-backed assets after a window context exists.
void Engine::LoadRuntimeAssets(RuntimeContext& runtime)
{
	runtime.skyBox = std::make_unique<SkyBox>(runtime.assetPaths.environmentTexturesDirectory.string());
	runtime.selectionCube = std::make_unique<BlockSelectionCube>(runtime.assetPaths.blockTexturesDirectory.string());
	runtime.plane = std::make_unique<Plane>((runtime.assetPaths.blockTexturesDirectory / "cobblestone.png").string());
	runtime.blockRegistry = std::make_unique<ve::blocks::BlockRegistry>(runtime.assetPaths);
	runtime.world = std::make_unique<ve::world::World>(ve::world::CreateInfoForSquareWorld(DefaultWorldSizeChunks));
	runtime.world->SpawnFlatGrid(ve::world::FlatWorldSpawnSettings{ DefaultWorldSizeChunks });
	runtime.hudRenderer = std::make_unique<ve::ui::HudRenderer>(runtime.assetPaths);
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
	UpdateFrameGameplay(runtime.window, *runtime.world, *runtime.blockRegistry, runtime.camera, runtime.currentSelection, runtime.frameTimer.DeltaSeconds());
	Render3DWorld(runtime.window, runtime.camera, *runtime.skyBox, *runtime.plane, *runtime.selectionCube, *runtime.blockRegistry, *runtime.world, runtime.currentSelection);
	runtime.hudRenderer->Draw(CreateHudFrame(runtime.window, runtime.camera, runtime.frameTimer, runtime.currentSelection, *runtime.blockRegistry, *runtime.world));
	runtime.window.Update();
}
