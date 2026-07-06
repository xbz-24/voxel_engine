#pragma once

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
#include "RuntimeInputRouter.h"
#include "SkyBox.h"
#include "Window.h"
#include "World.h"

#include <atomic>
#include <memory>
#include <optional>
#include <string>
#include <vector>

namespace ve::engine
{
	class EngineRuntime;
	class RenderView;
}

/** Coordinates engine settings, callbacks, and the runtime entry point. */
class EngineApplication
{
public:
	/** Creates the engine and records default runtime state. */
	EngineApplication();
	explicit EngineApplication(ve::engine::EngineCreateInfo create_info);
	explicit EngineApplication(ve::engine::ValidatedEngineCreateInfo create_info);

	/** Releases engine-owned resources. */
	~EngineApplication();

	/** Starts the runtime loop and returns a process-style status code. */
	int Run();

	/** Initializes runtime systems without entering the blocking loop. */
	[[nodiscard]] bool Start();

	/** Advances one externally driven frame. */
	[[nodiscard]] bool Step();

	/** Releases an externally driven runtime, if one is active. */
	void Shutdown() noexcept;

	/** Requests the active runtime loop to stop at the next frame boundary. */
	void RequestStop() noexcept;

	/** @return True when a stop was requested through the public engine handle. */
	[[nodiscard]] bool IsStopRequested() const noexcept;

	/** @return True when runtime systems are initialized and ready to step. */
	[[nodiscard]] bool IsRunning() const noexcept;

private:
	friend class ve::engine::EngineRuntime;

	/** Connects GLFW callbacks to engine-owned input state. */
	void ConfigureCallbacks(ve::engine::Window& window, ve::engine::RuntimeInputRouter& input_router);

	/** Configures global state used only by the OpenGL compatibility renderer. */
	void ConfigureOpenGLState();

	/** Initializes the window for the selected graphics backend. */
	[[nodiscard]] ve::engine::EngineStartupResult InitializeWindow(ve::engine::Window& window);

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

	/** Draws debug X/Y/Z axes in world space. */
	void renderDebugCoordinateSystemAxes();

	/** Draws the selected block outline in the OpenGL compatibility path. */
	void drawBlockHighlight(glm::ivec3 blockPos, BlockSelectionCube& selectionCube);

	/** Renders the legacy OpenGL 3D scene. */
	void Render3DWorld(ve::engine::RenderView& renderView, const ve::engine::Window& window, Camera& camera, SkyBox& skyBox, Plane& plane, BlockSelectionCube& selectionCube, const ve::blocks::BlockRegistry& blockRegistry, ve::world::World& world, const ve::gameplay::BlockSelection& selection);

	/** Rebuilds 3D and 2D projection matrices after window size changes. */
	void UpdateProjections(int width, int height);

	/** Applies a new framebuffer size when it differs from the cached size. */
	void ApplyFramebufferSize(int width, int height);

	/** @return Validated runtime create info. */
	[[nodiscard]] const ve::engine::EngineCreateInfo& CreateInfo() const noexcept;

	/** @return True when raw create info validation succeeded. */
	[[nodiscard]] bool HasValidCreateInfo() const noexcept;

	EngineRenderCacheState _render_cache_state;
	EngineWindowState _window_state;
	ve::gameplay::RuntimeSettings _runtimeSettings;
	std::optional<ve::engine::ValidatedEngineCreateInfo> create_info_;
	std::vector<std::string> create_info_validation_issues_;
	std::atomic_bool stop_requested_{ false };
	std::unique_ptr<ve::engine::EngineRuntime> runtime_;
};
