#include "Engine.h"

#include "AssetPaths.h"
#include "BlockRegistry.h"
#include "FrameTimer.h"
#include "HudRenderer.h"
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
	  _wasSettingsTogglePressed(false),
	  _wasSettingsUpPressed(false),
	  _wasSettingsDownPressed(false),
	  _wasSettingsLeftPressed(false),
	  _wasSettingsRightPressed(false),
	  _wasSettingsConfirmPressed(false),
	  _wasFlyTogglePressed(false),
	  _wasRenderDistanceDecreasePressed(false),
	  _wasRenderDistanceIncreasePressed(false),
	  _isSettingsMenuOpen(false),
	  _isVSyncEnabled(false),
	  _isDebugOverlayVisible(true),
	  _isFlying(false),
	  _isGrounded(false),
	  _verticalVelocity(0.0f),
	  _renderDistanceChunks(2),
	  _selectedSettingsMenuOption(ve::ui::SettingsMenuOption::RenderDistance),
	  _selectedPlacementBlock(ve::blocks::BlockId::Cobblestone)
{
	_applicationSourceFilePath = std::filesystem::absolute(__FILE__);
}

Engine::~Engine() = default;

int Engine::Run()
{
	Window window("Voxel Engine v1.0.0");
	if (!window.Initialize())
	{
		return -1;
	}
	window.SetVSync(_isVSyncEnabled);

	Camera camera;
	CallbackContext callbackContext{ &camera, &_isSettingsMenuOpen, { 0.0, 0.0, true } };
	ConfigureCallbacks(window, callbackContext);
	ConfigureOpenGLState();

	const ve::assets::AssetPaths assetPaths = ve::assets::ResolveFromSourceFile(_applicationSourceFilePath);
	SkyBox skyBox(assetPaths.environmentTexturesDirectory.string());
	Cube cube(assetPaths.blockTexturesDirectory.string());
	Plane plane((assetPaths.blockTexturesDirectory / "cobblestone.png").string());
	ve::blocks::BlockRegistry blockRegistry(assetPaths);

	const int worldSize = 10;
	ve::world::World world(static_cast<std::size_t>(worldSize * worldSize));
	world.SpawnFlatGrid(worldSize);

	ve::ui::HudRenderer hudRenderer(assetPaths);
	ve::time::FrameTimer frameTimer;
	BlockSelection currentSelection{ false, glm::ivec3(0), glm::ivec3(0) };

	while (!window.ShouldClose())
	{
		if (window.GetWidth() != _currentWindowWidth || window.GetHeight() != _currentWindowHeight)
		{
			_currentWindowWidth = window.GetWidth();
			_currentWindowHeight = window.GetHeight();
			UpdateProjections(_currentWindowWidth, _currentWindowHeight);
		}

		frameTimer.Tick();
		ProcessInput(window, world, blockRegistry, camera, frameTimer.DeltaSeconds());
		if (_isSettingsMenuOpen)
		{
			currentSelection.hasTarget = false;
		}
		else
		{
			UpdateGameLogic(world, blockRegistry, camera, currentSelection);
			ProcessGameplayInput(window, world, currentSelection);
			UpdateGameLogic(world, blockRegistry, camera, currentSelection);
		}
		Render3DWorld(window, camera, skyBox, plane, cube, blockRegistry, world, currentSelection);

		const ve::ui::HudFrameInfo hudFrame{
			window,
			camera,
			frameTimer.DisplayedFps(),
			currentSelection.targetBlock,
			currentSelection.hasTarget,
			blockRegistry,
			_selectedPlacementBlock,
			_isDebugOverlayVisible,
			_isFlying,
			_renderDistanceChunks,
			ve::ui::SettingsMenuState{ _isSettingsMenuOpen, _selectedSettingsMenuOption, _renderDistanceChunks, _isVSyncEnabled, _isDebugOverlayVisible, _isFlying }
		};
		hudRenderer.Draw(hudFrame);
		window.Update();
	}

	if (_cloudDisplayListID != 0)
	{
		glDeleteLists(_cloudDisplayListID, 1);
		_cloudDisplayListID = 0;
	}
	return 0;
}
