#include "RenderView.h"

namespace ve::engine
{
	/** Returns null when a render view has no primitive command executor. */
	ve::rendering::GraphicsFacade* RenderView::Graphics() noexcept { return nullptr; }

	/** Returns null when a render view has no primitive command executor. */
	const ve::rendering::GraphicsFacade* RenderView::Graphics() const noexcept { return nullptr; }

	/** Returns null when a render view has no skybox renderer. */
	SkyBox* RenderView::Skybox() noexcept { return nullptr; }

	/** Returns null when a render view has no ground plane renderer. */
	Plane* RenderView::GroundPlane() noexcept { return nullptr; }

	/** Returns null when a render view has no selected-block renderer. */
	BlockSelectionCube* RenderView::SelectionCube() noexcept { return nullptr; }

	/** Returns null when a render view has no HUD renderer. */
	ve::ui::HudRenderer* RenderView::Hud() noexcept { return nullptr; }

	void RenderView::RenderCloudLayer()
	{
	}

	void RenderView::ReleaseCachedResources()
	{
	}
}
