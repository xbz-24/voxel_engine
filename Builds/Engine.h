#pragma once

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include "BlockRegistry.h"
#include "Camera.h"
#include "BlockSelectionCube.h"
#include "FrameTimer.h"
#include "HudRenderer.h"
#include "RuntimeSettings.h"
#include "SettingsMenuController.h"
#include "Window.h"
#include "SkyBox.h"
#include "Plane.h"
#include "World.h"

#include <filesystem>
#include <string>

class Engine
{
public:
	/// Creates the engine and records the source file path used to resolve assets.
	Engine();
	/// Destroys the engine.
	~Engine();
	/// Starts the main loop and returns a process-style status code.
	int Run();
private:
	struct MouseState { double previousX, previousY; bool isFirstInputEvent; };
	struct CallbackContext { Camera* camera; const bool* isSettingsMenuOpen; MouseState mouse; };
	struct BlockSelection { bool hasTarget; glm::ivec3 targetBlock, placementBlock; };
	/// Receives GLFW mouse movement and applies it to the camera.
	static void mouse_callback(GLFWwindow* window, double xpos, double ypos);
	/// Configures GLFW callbacks and callback state for engine-owned systems.
	void ConfigureCallbacks(Window& window, CallbackContext& context);
	/// Configures global OpenGL state used by the current renderer.
	void ConfigureOpenGLState();
	/// Initializes a window and applies runtime window settings.
	bool InitializeWindow(Window& window);
	/// Initializes logger sinks that depend on resolved asset paths.
	void ConfigureRuntimeLogging(const ve::assets::AssetPaths& assetPaths);
	/// Updates projection matrices when the framebuffer dimensions change.
	void UpdateProjectionIfWindowChanged(const Window& window);
	/// Runs one frame of gameplay input and targeting.
	void UpdateFrameGameplay(Window& window, ve::world::World& world, const ve::blocks::BlockRegistry& blockRegistry, Camera& camera, BlockSelection& selection, double deltaSeconds);
	/// Builds immutable HUD data for the current frame.
	ve::ui::HudFrameInfo CreateHudFrame(const Window& window, const Camera& camera, const ve::time::FrameTimer& frameTimer, const BlockSelection& selection, const ve::blocks::BlockRegistry& blockRegistry, const ve::world::World& world) const;
	/// Releases cached OpenGL objects owned directly by the engine.
	void ReleaseRenderCaches();
	/// Draws debug X/Y/Z axes in world space.
	void renderDebugCoordinateSystemAxes();
	/// Reads keyboard input and moves or closes the window.
	void handlePlayerMovementAndWindowInput(GLFWwindow* window, const ve::world::World& world, const ve::blocks::BlockRegistry& blockRegistry, Camera& camera, double frameDeltaTimeSeconds);
	/// Applies lightweight vertical player physics.
	void ApplyPlayerPhysics(const ve::world::World& world, const ve::blocks::BlockRegistry& blockRegistry, Camera& camera, double frameDeltaTimeSeconds);
	/// Casts a ray from the camera to find the targeted block and placement cell.
	bool performRaycastToFindTargetBlock(const ve::world::World& world, const ve::blocks::BlockRegistry& blockRegistry, Camera& camera, BlockSelection& outSelection);
	/// Draws a wireframe outline around the selected block.
	void drawBlockHighlight(glm::ivec3 blockPos, BlockSelectionCube& selectionCube);
	/// Processes per-frame player input.
	void ProcessInput(Window& window, const ve::world::World& world, const ve::blocks::BlockRegistry& blockRegistry, Camera& camera, double frameTimeDeltaSeconds);
	/// Processes block interaction input for the selected target.
	void ProcessGameplayInput(Window& window, ve::world::World& world, const BlockSelection& selection);
	/// Updates simulation state for the current frame.
	void UpdateGameLogic(const ve::world::World& world, const ve::blocks::BlockRegistry& blockRegistry, Camera& camera, BlockSelection& selection);
	/// Renders the 3D scene.
	void Render3DWorld(const Window& window, Camera& camera, SkyBox& skyBox, Plane& plane, BlockSelectionCube& selectionCube, const ve::blocks::BlockRegistry& blockRegistry, ve::world::World& world, const BlockSelection& selection);
	/// Rebuilds 3D and 2D projection matrices after window size changes.
	void UpdateProjections(int width, int height);
	/// Draws a simple procedural cloud layer.
	void RenderClouds();
	/// Builds the cached cloud display list.
	void BuildCloudDisplayList();
	std::filesystem::path _applicationSourceFilePath;
	glm::mat4 _projection3D;
	int _currentWindowWidth;
	int _currentWindowHeight;
	GLuint _cloudDisplayListID;
	bool _wasLeftMouseButtonPressed;
	bool _wasRightMouseButtonPressed;
	bool _wasDebugTogglePressed;
	bool _wasFlyTogglePressed;
	bool _wasRenderDistanceDecreasePressed;
	bool _wasRenderDistanceIncreasePressed;
	bool _isGrounded;
	ve::gameplay::RuntimeSettings _runtimeSettings;
	ve::gameplay::SettingsMenuController _settingsMenuController;
	ve::blocks::BlockId _selectedPlacementBlock;
};
