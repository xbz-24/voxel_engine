#pragma once

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include <filesystem>
#include <string>
#include "AssetPaths.h"
#include "BlockRegistry.h"
#include "Camera.h"
#include "Cube.h"
#include "FrameTimer.h"
#include "HudRenderer.h"
#include "Window.h"
#include "SkyBox.h"
#include "Plane.h"
#include "World.h"

class Engine
{
public:
	/**
	 * Creates the engine and records the source file path used to resolve assets.
	 */
	Engine();

	/**
	 * Destroys the engine.
	 */
	~Engine();

	/**
	 * Starts the main loop.
	 *
	 * @return 0 when the window closes normally, or -1 when initialization fails.
	 */
	int Run();
private:
	struct MouseState
	{
		double previousX;
		double previousY;
		bool isFirstInputEvent;
	};

	struct CallbackContext
	{
		Camera* camera;
		MouseState mouse;
	};

	struct BlockSelection
	{
		bool hasTarget;
		glm::ivec3 targetBlock;
		glm::ivec3 placementBlock;
	};

	/**
	 * Receives GLFW mouse movement and applies it to the camera.
	 *
	 * @param window Native GLFW window carrying the camera user pointer.
	 * @param xpos Current cursor X position in pixels.
	 * @param ypos Current cursor Y position in pixels.
	 */
	static void mouse_callback(GLFWwindow* window, double xpos, double ypos);

	/**
	 * Configures GLFW callbacks and callback state for engine-owned systems.
	 *
	 * @param window Window whose native callbacks are configured.
	 * @param context Context made reachable from GLFW callbacks.
	 */
	void ConfigureCallbacks(Window& window, CallbackContext& context);

	/**
	 * Configures global OpenGL state used by the current renderer.
	 */
	void ConfigureOpenGLState();

	/**
	 * Draws debug X/Y/Z axes in world space.
	 */
	void renderDebugCoordinateSystemAxes();

	/**
	 * Reads keyboard input and moves or closes the window.
	 *
	 * @param window Native GLFW window used for key polling.
	 * @param world World queried for basic collision.
	 * @param blockRegistry Registry used to identify solid blocks.
	 * @param camera Camera moved by the player controls.
	 * @param frameDeltaTimeSeconds Seconds elapsed since the previous frame.
	 */
	void handlePlayerMovementAndWindowInput(GLFWwindow* window, const ve::world::World& world, const ve::blocks::BlockRegistry& blockRegistry, Camera& camera, double frameDeltaTimeSeconds);

	/**
	 * Applies lightweight vertical player physics.
	 *
	 * @param world World queried for ground collision.
	 * @param blockRegistry Registry used to identify solid blocks.
	 * @param camera Camera representing the player eye position.
	 * @param frameDeltaTimeSeconds Seconds elapsed since the previous frame.
	 */
	void ApplyPlayerPhysics(const ve::world::World& world, const ve::blocks::BlockRegistry& blockRegistry, Camera& camera, double frameDeltaTimeSeconds);

	/**
	 * Casts a ray from the camera to find the block currently targeted and placement cell.
	 *
	 * @param world World queried for block ids.
	 * @param blockRegistry Registry used to identify solid blocks.
	 * @param camera Camera that provides the ray origin and direction.
	 * @param out_selection Filled with target/placement coordinates when a hit is found.
	 * @return true when a block is hit, false otherwise.
	 */
	bool performRaycastToFindTargetBlock(const ve::world::World& world, const ve::blocks::BlockRegistry& blockRegistry, Camera& camera, BlockSelection& out_selection);

	/**
	 * Draws a wireframe outline around the selected block.
	 *
	 * @param blockPos Block coordinates to highlight.
	 * @param cube Cube renderer used to draw the outline geometry.
	 */
	void drawBlockHighlight(glm::ivec3 blockPos, Cube& cube);
	std::filesystem::path _applicationSourceFilePath;

	/**
	 * Processes per-frame player input.
	 *
	 * @param window Window wrapper used for native input access.
	 * @param camera Camera controlled by the player.
	 * @param frameTimeDeltaSeconds Seconds elapsed since the previous frame.
	 */
	void ProcessInput(Window& window, const ve::world::World& world, const ve::blocks::BlockRegistry& blockRegistry, Camera& camera, double frameTimeDeltaSeconds);

	/**
	 * Processes block interaction input for the selected target.
	 *
	 * @param window Window used for mouse/key polling.
	 * @param world World mutated by block placement and breaking.
	 * @param selection Current block raycast result.
	 */
	void ProcessGameplayInput(Window& window, ve::world::World& world, const BlockSelection& selection);

	/**
	 * Updates simulation state for the current frame.
	 *
	 * @param world World queried by raycasts.
	 * @param blockRegistry Registry used to identify solid blocks.
	 * @param camera Camera used for selection raycasts.
	 * @param selection Output selected target data.
	 */
	void UpdateGameLogic(const ve::world::World& world, const ve::blocks::BlockRegistry& blockRegistry, Camera& camera, BlockSelection& selection);

	/**
	 * Renders the 3D scene.
	 *
	 * @param window Window used for size-dependent rendering.
	 * @param camera Camera used to build the view matrix.
	 * @param skyBox Skybox renderer.
	 * @param plane Ground plane renderer.
	 * @param cube Cube renderer shared by chunks and selection.
	 * @param blockRegistry Registry used by chunk rendering.
	 * @param world Level chunk container and allocator arena.
	 * @param selection Current block raycast result.
	 */
	void Render3DWorld(const Window& window, Camera& camera, SkyBox& skyBox, Plane& plane, Cube& cube, const ve::blocks::BlockRegistry& blockRegistry, ve::world::World& world, const BlockSelection& selection);

	/**
	 * Rebuilds 3D and 2D projection matrices after window size changes.
	 *
	 * @param width Framebuffer width in pixels.
	 * @param height Framebuffer height in pixels.
	 */
	void UpdateProjections(int width, int height);
	glm::mat4 _projection3D;
	int _currentWindowWidth;
	int _currentWindowHeight;

	/**
	 * Draws a simple procedural cloud layer.
	 */
	void RenderClouds();

	bool _wasLeftMouseButtonPressed;
	bool _wasRightMouseButtonPressed;
	bool _wasDebugTogglePressed;
	bool _wasFlyTogglePressed;
	bool _wasRenderDistanceDecreasePressed;
	bool _wasRenderDistanceIncreasePressed;
	bool _isDebugOverlayVisible;
	bool _isFlying;
	bool _isGrounded;
	float _verticalVelocity;
	int _renderDistanceChunks;
	ve::blocks::BlockId _selectedPlacementBlock;

};

