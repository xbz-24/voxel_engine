#pragma once

#include "AssetPaths.h"
#include "BlockRegistry.h"
#include "BlockSelectionCube.h"
#include "HudRenderer.h"
#include "ModelAssetLibrary.h"
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
		/** @return Model importer library used by demos and tools. */
		ve::assets::ModelAssetLibrary& MutableModelAssets() noexcept;
		/** @return HUD renderer used for 2D overlays. */
		ve::ui::HudRenderer& MutableHudRenderer() noexcept;

	private:
		SkyBox sky_box_;
		BlockSelectionCube selection_cube_;
		Plane plane_;
		ve::blocks::BlockRegistry block_registry_;
		ve::assets::ModelAssetLibrary model_asset_library_;
		ve::ui::HudRenderer hud_renderer_;
	};
}
