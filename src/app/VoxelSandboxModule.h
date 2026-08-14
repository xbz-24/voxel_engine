#pragma once

#include "EditorRuntimeController.h"
#include "EngineRuntimeCallbacks.h"
#include "EngineState.h"
#include "GameController.h"
#include "RuntimeInput.h"
#include "RuntimeInputRouter.h"
#include "RuntimeModule.h"
#include "RuntimeRenderHost.h"
#include "RuntimeSettings.h"
#include "StaticModelSceneRuntime.h"

#include <memory>

class EngineApplication;
class BlockSelectionCube;

namespace ve::engine
{
	class GameModel;
	class RuntimeRenderDriver;

	/** Owns voxel gameplay, world, editor, callbacks, and rendering integration. */
	class VoxelSandboxModule final : public IRuntimeModule, private RuntimeRenderHost
	{
	public:
		explicit VoxelSandboxModule(EngineApplication& engine) noexcept;
		~VoxelSandboxModule() override;
		[[nodiscard]] EngineStartupResult Initialize(RuntimeModuleContext& context) override;
		void RunFrame(RuntimeModuleContext& context) override;
		void Shutdown() noexcept override;

	private:
		[[nodiscard]] EngineStartupResult CreateRenderDriver(RuntimeModuleContext& context);
		[[nodiscard]] EngineStartupResult CreateStaticModelScene();
		void InvokePublicApiFrameCallbacks(RuntimeModuleContext& context);
		void ApplyConfiguredWorldEditsOnce();
		void ApplyWorldEdits(const std::vector<WorldBlockEdit>& edits);
		void ConfigureInputCallbacks();
		void UpdateProjectionIfWindowChanged();
		void ApplyFramebufferSize(int width, int height);
		void UpdateProjections(int width, int height);
		void RenderDebugCoordinateSystemAxes();
		void DrawBlockHighlight(glm::ivec3 block_position, BlockSelectionCube& selection_cube);
		void UpdateViewportProjection() override;
		void RenderWorld(RenderView& view) override;
		void RenderStaticModelScene() override;
		void RenderHud(RenderView& view) override;

		EngineApplication& engine_;
		RuntimeInputRouter input_router_;
		std::unique_ptr<GameModel> model_;
		std::unique_ptr<RuntimeRenderDriver> render_driver_;
		StaticModelSceneRuntime static_model_scene_;
		RuntimeInputActionTracker runtime_input_actions_;
		GameController controller_;
		ve::editor::EditorRuntimeController editor_controller_;
		ve::gameplay::RuntimeSettings runtime_settings_;
		EngineRenderCacheState render_cache_state_;
		EngineWindowState window_state_;
		double elapsed_seconds_ = 0.0;
		bool configured_world_edits_applied_ = false;
		Window* window_ = nullptr;
		const ve::time::FrameTimer* frame_timer_ = nullptr;
	};
}
