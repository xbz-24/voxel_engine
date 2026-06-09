#include "GameView.h"

namespace ve::engine
{
	/// Loads OpenGL-backed view resources.
	GameView::GameView(const ve::assets::AssetPaths& asset_paths)
		: sky_box_(asset_paths.environmentTexturesDirectory.string()),
		  selection_cube_(asset_paths.blockTexturesDirectory.string()),
		  plane_((asset_paths.blockTexturesDirectory / "cobblestone.png").string()),
		  block_registry_(asset_paths),
		  hud_renderer_(asset_paths)
	{
	}

	/// Returns the mutable skybox renderer.
	SkyBox& GameView::MutableSkyBox() noexcept { return sky_box_; }

	/// Returns the mutable selected-block outline mesh.
	BlockSelectionCube& GameView::MutableSelectionCube() noexcept { return selection_cube_; }

	/// Returns the mutable ground/debug plane renderer.
	Plane& GameView::MutablePlane() noexcept { return plane_; }

	/// Returns the block registry used by world rendering and HUD.
	ve::blocks::BlockRegistry& GameView::MutableBlockRegistry() noexcept { return block_registry_; }

	/// Returns the HUD renderer used for 2D overlays.
	ve::ui::HudRenderer& GameView::MutableHudRenderer() noexcept { return hud_renderer_; }
}
