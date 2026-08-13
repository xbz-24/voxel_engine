#include "EngineRuntime.h"

#include "RenderView.h"

#include <cassert>

namespace ve::engine
{
	void EngineRuntime::UpdateViewportProjection()
	{
		engine_.UpdateProjectionIfWindowChanged(window_);
	}

	void EngineRuntime::RenderWorld(RenderView& render_view)
	{
		ve::blocks::BlockRegistry* registry = model_->MutableBlockRegistry();
		assert(registry != nullptr);
		SkyBox* skybox = render_view.Skybox();
		Plane* plane = render_view.GroundPlane();
		BlockSelectionCube* selection = render_view.SelectionCube();
		assert(skybox != nullptr);
		assert(plane != nullptr);
		assert(selection != nullptr);
		engine_.Render3DWorld(render_view, window_, model_->MutableCamera(), *skybox, *plane,
			*selection, *registry, model_->MutableWorld(), model_->GetSelection());
	}

	void EngineRuntime::RenderHud(RenderView& render_view)
	{
		const ve::blocks::BlockRegistry* registry = model_->GetBlockRegistry();
		assert(registry != nullptr);
		ve::ui::HudRenderer* hud = render_view.Hud();
		assert(hud != nullptr);
		hud->Draw(engine_.CreateHudFrame(window_, model_->GetCamera(), frame_timer_,
			model_->GetSelection(), *registry, model_->GetWorld(),
			controller_.SelectedPlacementBlock()));
	}
}
