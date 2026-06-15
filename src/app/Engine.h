#pragma once

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include "BlockSelection.h"
#include "BlockRegistry.h"
#include "Camera.h"
#include "BlockSelectionCube.h"
#include "EngineState.h"
#include "FrameTimer.h"
#include "HudRenderer.h"
#include "RuntimeSettings.h"
#include "SettingsMenuController.h"
#include "Window.h"
#include "SkyBox.h"
#include "Plane.h"
#include "World.h"

#include <memory>
#include <string>

namespace ve::engine
{
	class GameController;
	class EngineRuntime;
}

/**
* @class Engine
* @brief The core orchestration class for the voxel engine.
* * @details This class acts as the central hub, managing the lifecycle of the window, 
* OpenGL context, input handling, and the primary game loop. It bridges the gap 
* between the OS-level events (via GLFW), the physical simulation, and the rendering pipeline.
*/
class Engine
{
public:

	/**
	* @brief Constructs the engine and establishes the baseline environment state.
	* * @details Records the executable's source file path to safely resolve relative asset 
	* directories (like textures and shaders) regardless of the current working directory 
	* from which the program was launched.
	*/
	Engine();

	/**
	* @brief Destroys the engine and safely releases resources.
	* * @details Ensures that all engine-owned OpenGL buffers, window contexts, and 
	* logging sinks are gracefully terminated to prevent memory or VRAM leaks.
	*/
	~Engine();

	/**
	* @brief Enters the main application loop.
	* * @details Orchestrates frame timing, input polling, state updates, and rendering. 
	* This method blocks the calling thread until the window is flagged to close.
	* * @return A standard process exit code (e.g., 0 for success, non-zero for fatal errors).
	*/
	int Run();
	
private:

	friend class ve::engine::GameController;
	friend class ve::engine::EngineRuntime;

	/**
	* @struct MouseState
	* @brief Tracks the delta movements of the mouse cursor for camera control.
	*/
	struct MouseState { double previousX, previousY; bool isFirstInputEvent; };

	/**
	* @struct CallbackContext
	* @brief A payload container used to pass C++ instance state into GLFW's C-style callbacks.
	*/
	struct CallbackContext { Camera* camera; const bool* isSettingsMenuOpen; MouseState mouse; };

	/**
	* @brief Translates raw OS cursor movement into camera orientation updates.
	*
	* @details This is a static callback required by GLFW. It calculates the delta between 
	* the current and previous cursor positions to update the camera's pitch and yaw.
	*
	* @param window The GLFW window receiving the input.
	* @param xpos The new absolute X position of the cursor.
	* @param ypos The new absolute Y position of the cursor.
	*/
	static void mouse_callback(GLFWwindow* window, double xpos, double ypos) noexcept;

	/**
	* @brief Binds GLFW callbacks to internal engine handlers.
	*
	* @details Sets up the user pointer in GLFW so that static C-callbacks can interact 
	* with the non-static `CallbackContext`, ensuring input is routed to the active `Camera`.
	*
	* @param window The wrapper containing the GLFWwindow pointer.
	* @param context The state block to inject into the GLFW window user pointer.
	*/
	void ConfigureCallbacks(Window& window, CallbackContext& context);

	/**
	*
	* @brief Establishes the global OpenGL state machine parameters.
	*
	* @details Enables depth testing, face culling, and blending. This is necessary 
	* because OpenGL is a state machine; setting this guarantees a predictable baseline 
	* before any 3D rendering occurs.
	*/
	void ConfigureOpenGLState();

	/**
	 * @brief Initializes the OS window and the OpenGL context.
	 * * @details Applies user-defined `RuntimeSettings` (e.g., resolution, VSync, fullscreen) 
	 * to configure the display. Must be called before any OpenGL functions are used.
	 * * @param window The window wrapper to initialize.
	 * @return True if window and context creation succeeded, false otherwise.
	 */
	bool InitializeWindow(Window& window);

	/// Initializes logger sinks that depend on resolved asset paths.
	void ConfigureRuntimeLogging(const ve::assets::AssetPaths& assetPaths);

	/// Updates projection matrices when the framebuffer dimensions change.
	void UpdateProjectionIfWindowChanged(const Window& window);

	/// Runs one frame of gameplay input and targeting.
	void UpdateFrameGameplay(Window& window, ve::world::World& world, const ve::blocks::BlockRegistry& blockRegistry, Camera& camera, ve::gameplay::BlockSelection& selection, double deltaSeconds);

	/// Builds immutable HUD data for the current frame.
	ve::ui::HudFrameInfo CreateHudFrame(const Window& window, const Camera& camera, const ve::time::FrameTimer& frameTimer, const ve::gameplay::BlockSelection& selection, const ve::blocks::BlockRegistry& blockRegistry, const ve::world::World& world) const;

	/// Releases cached OpenGL objects owned directly by the engine.
	void ReleaseRenderCaches();

	/// Draws debug X/Y/Z axes in world space.
	void renderDebugCoordinateSystemAxes();

	/// Reads keyboard input and moves or closes the window.
	void handlePlayerMovementAndWindowInput(GLFWwindow* window, const ve::world::World& world, const ve::blocks::BlockRegistry& blockRegistry, Camera& camera, double frameDeltaTimeSeconds);

	/// Applies lightweight vertical player physics.
	void ApplyPlayerPhysics(const ve::world::World& world, const ve::blocks::BlockRegistry& blockRegistry, Camera& camera, double frameDeltaTimeSeconds);

	/// Casts a ray from the camera to find the targeted block and placement cell.
	bool performRaycastToFindTargetBlock(const ve::world::World& world, const ve::blocks::BlockRegistry& blockRegistry, Camera& camera, ve::gameplay::BlockSelection& outSelection);

	/// Draws a wireframe outline around the selected block.
	void drawBlockHighlight(glm::ivec3 blockPos, BlockSelectionCube& selectionCube);

	/// Processes per-frame player input.
	void ProcessInput(Window& window, const ve::world::World& world, const ve::blocks::BlockRegistry& blockRegistry, Camera& camera, double frameTimeDeltaSeconds);

	/// Processes block interaction input for the selected target.
	void ProcessGameplayInput(Window& window, ve::world::World& world, const ve::gameplay::BlockSelection& selection);

	/// Updates simulation state for the current frame.
	void UpdateGameLogic(const ve::world::World& world, const ve::blocks::BlockRegistry& blockRegistry, Camera& camera, ve::gameplay::BlockSelection& selection);

	/// Renders the 3D scene.
	void Render3DWorld(const Window& window, Camera& camera, SkyBox& skyBox, Plane& plane, BlockSelectionCube& selectionCube, const ve::blocks::BlockRegistry& blockRegistry, ve::world::World& world, const ve::gameplay::BlockSelection& selection);

	/// Rebuilds 3D and 2D projection matrices after window size changes.
	void UpdateProjections(int width, int height);

	/// Draws a simple procedural cloud layer.
	void RenderClouds();

	/// Builds the cached cloud display list.
	void BuildCloudDisplayList();
	
	EngineRenderCacheState _render_cache_state;
	EngineWindowState _window_state;
	EngineInputState _input_state;
	ve::gameplay::RuntimeSettings _runtimeSettings;
	ve::gameplay::SettingsMenuController _settingsMenuController;
	ve::blocks::BlockId _selectedPlacementBlock;
};
