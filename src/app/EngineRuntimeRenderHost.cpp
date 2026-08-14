#include "VoxelSandboxModule.h"

#include "BlockSelectionCube.h"
#include "FrameTimer.h"
#include "GameModel.h"
#include "HudFrameInfo.h"
#include "HudRenderer.h"
#include "Plane.h"
#include "RenderState.h"
#include "RenderView.h"
#include "SkyBox.h"
#include "World.h"

#include <cassert>

namespace ve::engine
{
	void VoxelSandboxModule::UpdateViewportProjection()
	{
		UpdateProjectionIfWindowChanged();
	}

	void VoxelSandboxModule::RenderWorld(RenderView& render_view)
	{
		ve::blocks::BlockRegistry* registry = model_->MutableBlockRegistry();
		assert(registry != nullptr);
		SkyBox* skybox = render_view.Skybox();
		Plane* plane = render_view.GroundPlane();
		BlockSelectionCube* selection = render_view.SelectionCube();
		assert(skybox != nullptr);
		assert(plane != nullptr);
		assert(selection != nullptr);
		ve::rendering::BeginWorldFrame(0.541f, 0.694f, 0.976f);
		ve::rendering::ApplyProjection(render_cache_state_.projection_3d);
		const glm::mat4 camera_view = model_->MutableCamera().GetWorldToViewMatrix();
		ve::rendering::ApplyView(camera_view);
		ve::rendering::UseSolidFillMode();
		ve::rendering::UseBackFaceCulling();
		render_view.RenderCloudLayer();
		model_->MutableWorld().Draw(ve::world::WorldRenderRequest{ *registry,
			model_->MutableCamera().GetPosition(), model_->MutableCamera().GetForward(),
			render_cache_state_.projection_3d * camera_view,
			runtime_settings_.renderer.render_distance_chunks });
		RenderDebugCoordinateSystemAxes();
		if (model_->GetSelection().has_target)
			DrawBlockHighlight(model_->GetSelection().target_block, *selection);
	}

	void VoxelSandboxModule::RenderHud(RenderView& render_view)
	{
		const ve::blocks::BlockRegistry* registry = model_->GetBlockRegistry();
		assert(registry != nullptr);
		ve::ui::HudRenderer* hud = render_view.Hud();
		assert(hud != nullptr);
		hud->Draw(ve::ui::HudFrameInfo{ *window_, model_->GetCamera(),
			frame_timer_->DisplayedFps(), model_->GetSelection().target_block,
			model_->GetSelection().has_target, *registry,
			controller_.SelectedPlacementBlock(), runtime_settings_.renderer.show_debug_overlay,
			runtime_settings_.player.is_flying, runtime_settings_.renderer.render_distance_chunks,
			model_->GetWorld().PendingEventCount(), ve::gameplay::ToSettingsMenuState(runtime_settings_) });
	}

	void VoxelSandboxModule::RenderStaticModelScene()
	{
		static_model_scene_.Draw();
	}
}
