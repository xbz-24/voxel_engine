#pragma once

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include "BlockRegistry.h"
#include "BlockSelection.h"
#include "BlockSelectionCube.h"
#include "Camera.h"
#include "EngineState.h"
#include "FrameTimer.h"
#include "HudRenderer.h"
#include "Plane.h"
#include "RuntimeSettings.h"
#include "SettingsMenuController.h"
#include "SkyBox.h"
#include "Window.h"
#include "World.h"

#include <memory>
#include <string>

namespace ve::engine
{
	class GameController;
	class EngineRuntime;
}

/** Coordinates engine settings, callbacks, and the runtime entry point. */
class Engine
{
public:
	/** Creates the engine and records default runtime state. */
	Engine();

	/** Releases engine-owned resources. */
	~Engine();

	/** Starts the runtime loop and returns a process-style status code. */
	int Run();

private:
	friend class ve::engine::GameController;
	friend class ve::engine::EngineRuntime;

	struct MouseState
	{
		double previousX;
		double previousY;
		bool isFirstInputEvent;
	};

	struct CallbackContext
	{
		Camera* camera;
		const bool* isSettingsMenuOpen;
		MouseState mouse;
	};

	static void mouse_callback(GLFWwindow* window, double xpos, double ypos) noexcept;

	/** Connects GLFW callbacks to engine-owned input state. */
	void ConfigureCallbacks(ve::engine::Window& window, CallbackContext& context);

	/** Configures global state used only by the OpenGL compatibility renderer. */
	void ConfigureOpenGLState();

	/** Initializes the window for the selected graphics backend. */
	bool InitializeWindow(ve::engine::Window& window);

	/** Configures console and file logging after asset paths are resolved. */
	void ConfigureRuntimeLogging(const ve::assets::AssetPaths& assetPaths);

	/** Rebuilds cached projection matrices when the framebuffer size changes. */
	void UpdateProjectionIfWindowChanged(const ve::engine::Window& window);

	/** Runs one OpenGL gameplay tick, including movement, targeting, and block edits. */
	void UpdateFrameGameplay(ve::engine::Window& window, ve::world::World& world, const ve::blocks::BlockRegistry& blockRegistry, Camera& camera, ve::gameplay::BlockSelection& selection, double deltaSeconds);

	/** Builds immutable HUD data for the current OpenGL frame. */
	ve::ui::HudFrameInfo CreateHudFrame(const ve::engine::Window& window, const Camera& camera, const ve::time::FrameTimer& frameTimer, const ve::gameplay::BlockSelection& selection, const ve::blocks::BlockRegistry& blockRegistry, const ve::world::World& world) const;

	/** Releases OpenGL objects still owned directly by Engine. */
	void ReleaseRenderCaches();

	/** Draws debug X/Y/Z axes in world space. */
	void renderDebugCoordinateSystemAxes();

	/** Reads player movement input and applies OpenGL-path collision physics. */
	void handlePlayerMovementAndWindowInput(GLFWwindow* window, const ve::world::World& world, const ve::blocks::BlockRegistry& blockRegistry, Camera& camera, double frameDeltaTimeSeconds);

	/** Applies lightweight vertical collision and gravity for the legacy player controller. */
	void ApplyPlayerPhysics(const ve::world::World& world, const ve::blocks::BlockRegistry& blockRegistry, Camera& camera, double frameDeltaTimeSeconds);

	/** Casts from the camera to find the selected block and placement position. */
	bool performRaycastToFindTargetBlock(const ve::world::World& world, const ve::blocks::BlockRegistry& blockRegistry, Camera& camera, ve::gameplay::BlockSelection& outSelection);

	/** Draws the selected block outline in the OpenGL compatibility path. */
	void drawBlockHighlight(glm::ivec3 blockPos, BlockSelectionCube& selectionCube);

	/** Routes movement and menu input for the OpenGL compatibility path. */
	void ProcessInput(ve::engine::Window& window, const ve::world::World& world, const ve::blocks::BlockRegistry& blockRegistry, Camera& camera, double frameTimeDeltaSeconds);

	/** Handles block break/place actions for the selected target. */
	void ProcessGameplayInput(ve::engine::Window& window, ve::world::World& world, const ve::gameplay::BlockSelection& selection);

	/** Refreshes selection state for the current frame. */
	void UpdateGameLogic(const ve::world::World& world, const ve::blocks::BlockRegistry& blockRegistry, Camera& camera, ve::gameplay::BlockSelection& selection);

	/** Renders the legacy OpenGL 3D scene. */
	void Render3DWorld(const ve::engine::Window& window, Camera& camera, SkyBox& skyBox, Plane& plane, BlockSelectionCube& selectionCube, const ve::blocks::BlockRegistry& blockRegistry, ve::world::World& world, const ve::gameplay::BlockSelection& selection);

	/** Rebuilds 3D and 2D projection matrices after window size changes. */
	void UpdateProjections(int width, int height);

	/** Draws the cached OpenGL cloud layer. */
	void RenderClouds();

	/** Builds the OpenGL display list used by the cloud layer. */
	void BuildCloudDisplayList();

	EngineRenderCacheState _render_cache_state;
	EngineWindowState _window_state;
	EngineInputState _input_state;
	ve::gameplay::RuntimeSettings _runtimeSettings;
	ve::gameplay::SettingsMenuController _settingsMenuController;
	ve::blocks::BlockId _selectedPlacementBlock;
};
