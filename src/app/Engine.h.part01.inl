
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

	/** Initializes runtime systems and keeps the startup failure details. */
	[[nodiscard]] ve::engine::EngineStartupResult StartDetailed();

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
