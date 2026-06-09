#pragma once

#include "AssetPaths.h"
#include "BlockRegistry.h"
#include "BlockSelectionCube.h"
#include "HudRenderer.h"
#include "Plane.h"
#include "SkyBox.h"

namespace ve::engine
{
	class GameView
	{
	public:
		/**
		 * Loads OpenGL-backed view resources.
		 *
		 * @param asset_paths Resolved asset paths used by render resources.
		 */
		explicit GameView(const ve::assets::AssetPaths& asset_paths);

		/** @return Mutable skybox renderer. */
		SkyBox& MutableSkyBox() noexcept;
		/** @return Mutable selected-block outline mesh. */
		BlockSelectionCube& MutableSelectionCube() noexcept;
		/** @return Mutable ground/debug plane renderer. */
		Plane& MutablePlane() noexcept;
		/** @return Block registry used by world rendering and HUD. */
		ve::blocks::BlockRegistry& MutableBlockRegistry() noexcept;
		/** @return HUD renderer used for 2D overlays. */
		ve::ui::HudRenderer& MutableHudRenderer() noexcept;

	private:
		SkyBox sky_box_;
		BlockSelectionCube selection_cube_;
		Plane plane_;
		ve::blocks::BlockRegistry block_registry_;
		ve::ui::HudRenderer hud_renderer_;
	};
}
