#include "Engine.h"

#include "AssetPaths.h"
#include "BlockRegistry.h"
#include "FrameTimer.h"
#include "HudRenderer.h"
#include "Logger.h"
#include "Plane.h"
#include "SkyBox.h"
#include "Window.h"
#include "World.h"

#include <filesystem>

Engine::Engine()
	: _currentWindowHeight(0),
	  _currentWindowWidth(0),
	  _cloudDisplayListID(0),
	  _wasLeftMouseButtonPressed(false),
	  _wasRightMouseButtonPressed(false),
	  _wasDebugTogglePressed(false),
	  _wasFlyTogglePressed(false),
	  _wasRenderDistanceDecreasePressed(false),
	  _wasRenderDistanceIncreasePressed(false),
	  _isGrounded(false),
	  _selectedPlacementBlock(ve::blocks::BlockId::Cobblestone)
{
	_applicationSourceFilePath = std::filesystem::absolute(__FILE__);
}

Engine::~Engine() = default;

int Engine::Run()
{
	Window window("Voxel Engine v1.0.0");
	if (!InitializeWindow(window))
	{
		return -1;
	}
	Camera camera;
	CallbackContext callbackContext{ &camera, &_runtimeSettings.isSettingsMenuOpen, { 0.0, 0.0, true } };
	ConfigureCallbacks(window, callbackContext);
	ConfigureOpenGLState();

	const ve::assets::AssetPaths assetPaths = ve::assets::ResolveFromSourceFile(_applicationSourceFilePath);
	ConfigureRuntimeLogging(assetPaths);
	SkyBox skyBox(assetPaths.environmentTexturesDirectory.string());
	BlockSelectionCube selectionCube(assetPaths.blockTexturesDirectory.string());
	Plane plane((assetPaths.blockTexturesDirectory / "cobblestone.png").string());
	ve::blocks::BlockRegistry blockRegistry(assetPaths);

	const int worldSize = 10;
	ve::world::World world(ve::world::CreateInfoForSquareWorld(worldSize));
	world.SpawnFlatGrid(ve::world::FlatWorldSpawnSettings{ worldSize });

	ve::ui::HudRenderer hudRenderer(assetPaths);
	ve::time::FrameTimer frameTimer;
	BlockSelection currentSelection{ false, glm::ivec3(0), glm::ivec3(0) };

	while (!window.ShouldClose())
	{
		UpdateProjectionIfWindowChanged(window);
		frameTimer.Tick();
		UpdateFrameGameplay(window, world, blockRegistry, camera, currentSelection, frameTimer.DeltaSeconds());
		Render3DWorld(window, camera, skyBox, plane, selectionCube, blockRegistry, world, currentSelection);
		hudRenderer.Draw(CreateHudFrame(window, camera, frameTimer, currentSelection, blockRegistry, world));
		window.Update();
	}

	ReleaseRenderCaches();
	VE_LOG_INFO("Engine runtime stopped");
	return 0;
}
