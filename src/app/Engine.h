#pragma once

#include "BlockRegistry.h"
#include "BlockSelection.h"
#include "BlockSelectionCube.h"
#include "Camera.h"
#include "EngineApplicationState.h"
#include "FrameTimer.h"
#include "HudRenderer.h"
#include "Plane.h"
#include "RuntimeInputRouter.h"
#include "SkyBox.h"
#include "Window.h"
#include "World.h"

namespace ve::engine
{
	class EngineRuntime;
	class RenderView;
}

/** Coordinates engine settings, callbacks, and the runtime entry point. */
class EngineApplication : private ve::engine::EngineApplicationState
{
public:
	EngineApplication();
	explicit EngineApplication(ve::engine::EngineCreateInfo create_info);
	explicit EngineApplication(ve::engine::ValidatedEngineCreateInfo create_info);

	~EngineApplication();

	int Run();

	[[nodiscard]] bool Start();

	[[nodiscard]] ve::engine::EngineStartupResult StartDetailed();

	[[nodiscard]] bool Step();

	void Shutdown() noexcept;

	void RequestStop() noexcept;

	[[nodiscard]] bool IsStopRequested() const noexcept;

	[[nodiscard]] bool IsRunning() const noexcept;

private:
	friend class ve::engine::EngineRuntime;

	void ConfigureCallbacks(ve::engine::Window& window, ve::engine::RuntimeInputRouter& input_router);

	void ConfigureOpenGLState();

	[[nodiscard]] ve::engine::EngineStartupResult InitializeWindow(ve::engine::Window& window);

	void ConfigureRuntimeLogging(const ve::assets::AssetPaths& assetPaths);

	void UpdateProjectionIfWindowChanged(ve::engine::Window& window);

	/** Builds immutable HUD data for the current OpenGL frame. */
	ve::ui::HudFrameInfo CreateHudFrame(const ve::engine::Window& window,
		const Camera& camera,
		const ve::time::FrameTimer& frame_timer,
		const ve::gameplay::BlockSelection& selection,
		const ve::blocks::BlockRegistry& block_registry,
		const ve::world::World& world,
		ve::blocks::BlockId selected_placement_block) const;

	void renderDebugCoordinateSystemAxes();

	void drawBlockHighlight(glm::ivec3 blockPos, BlockSelectionCube& selectionCube);

	void Render3DWorld(ve::engine::RenderView& renderView, const ve::engine::Window& window, Camera& camera, SkyBox& skyBox, Plane& plane, BlockSelectionCube& selectionCube, const ve::blocks::BlockRegistry& blockRegistry, ve::world::World& world, const ve::gameplay::BlockSelection& selection);

	void UpdateProjections(int width, int height);

	void ApplyFramebufferSize(int width, int height);

	[[nodiscard]] const ve::engine::EngineCreateInfo& CreateInfo() const noexcept;

	[[nodiscard]] bool HasValidCreateInfo() const noexcept;

	[[nodiscard]] ve::gameplay::RuntimeSettings& MutableRuntimeSettings() noexcept;

	[[nodiscard]] const ve::gameplay::RuntimeSettings& RuntimeSettings() const noexcept;
};
