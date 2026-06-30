#pragma once

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include "BlockRegistry.h"
#include "BlockSelection.h"
#include "BlockSelectionCube.h"
#include "Camera.h"
#include "EngineConfiguration.h"
#include "EngineState.h"
#include "FrameTimer.h"
#include "HudRenderer.h"
#include "Plane.h"
#include "RuntimeSettings.h"
#include "SkyBox.h"
#include "Window.h"
#include "World.h"

#include <memory>
#include <string>

namespace ve::engine
{
	class EngineRuntime;
}

/** Coordinates engine settings, callbacks, and the runtime entry point. */
class EngineApplication
{
public:
	/** Creates the engine and records default runtime state. */
	EngineApplication();
	explicit EngineApplication(ve::engine::EngineCreateInfo create_info);

	/** Releases engine-owned resources. */
	~EngineApplication();

	/** Starts the runtime loop and returns a process-style status code. */
	int Run();

private:
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

	/** Rebuilds cached projection matrices from queued framebuffer resize events. */
	void UpdateProjectionIfWindowChanged(ve::engine::Window& window);

	/** Builds immutable HUD data for the current OpenGL frame. */
	ve::ui::HudFrameInfo CreateHudFrame(const ve::engine::Window& window,
		const Camera& camera,
		const ve::time::FrameTimer& frame_timer,
		const ve::gameplay::BlockSelection& selection,
		const ve::blocks::BlockRegistry& block_registry,
		const ve::world::World& world,
		ve::blocks::BlockId selected_placement_block) const;

	/** Releases OpenGL objects still owned directly by Engine. */
	void ReleaseRenderCaches();

	/** Draws debug X/Y/Z axes in world space. */
	void renderDebugCoordinateSystemAxes();

	/** Draws the selected block outline in the OpenGL compatibility path. */
	void drawBlockHighlight(glm::ivec3 blockPos, BlockSelectionCube& selectionCube);

	/** Renders the legacy OpenGL 3D scene. */
	void Render3DWorld(const ve::engine::Window& window, Camera& camera, SkyBox& skyBox, Plane& plane, BlockSelectionCube& selectionCube, const ve::blocks::BlockRegistry& blockRegistry, ve::world::World& world, const ve::gameplay::BlockSelection& selection);

	/** Rebuilds 3D and 2D projection matrices after window size changes. */
	void UpdateProjections(int width, int height);

	/** Applies a new framebuffer size when it differs from the cached size. */
	void ApplyFramebufferSize(int width, int height);

	/** Draws the cached OpenGL cloud layer. */
	void RenderClouds();

	/** Builds the OpenGL display list used by the cloud layer. */
	void BuildCloudDisplayList();

	EngineRenderCacheState _render_cache_state;
	EngineWindowState _window_state;
	ve::gameplay::RuntimeSettings _runtimeSettings;
	ve::engine::EngineCreateInfo create_info_;
};
