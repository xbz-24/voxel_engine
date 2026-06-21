#include "OpenGLRenderView.h"

namespace ve::engine
{
	/** Loads OpenGL-backed view resources. */
	OpenGLRenderView::OpenGLRenderView(const ve::assets::AssetPaths& asset_paths)
		: sky_box_(asset_paths.environmentTexturesDirectory.string()),
		  selection_cube_(asset_paths.blockTexturesDirectory.string()),
		  plane_((asset_paths.blockTexturesDirectory / "cobblestone.png").string()),
		  hud_renderer_(asset_paths),
		  graphics_(primitive_command_executor_)
	{
	}

	/** Returns the API represented by this view. */
	ve::rendering::GraphicsApi OpenGLRenderView::Api() const noexcept { return ve::rendering::GraphicsApi::OpenGLCompatibility; }

	/** Returns the high-level primitive drawing facade. */
	ve::rendering::GraphicsFacade* OpenGLRenderView::Graphics() noexcept { return &graphics_; }

	/** Returns the read-only high-level primitive drawing facade. */
	const ve::rendering::GraphicsFacade* OpenGLRenderView::Graphics() const noexcept { return &graphics_; }

	/** Returns this object for explicit legacy renderer access. */
	OpenGLRenderView* OpenGLRenderView::AsOpenGLRenderView() noexcept { return this; }

	/** Returns this object for explicit legacy renderer access. */
	const OpenGLRenderView* OpenGLRenderView::AsOpenGLRenderView() const noexcept { return this; }

	/** Returns the mutable skybox renderer. */
	SkyBox& OpenGLRenderView::MutableSkyBox() noexcept { return sky_box_; }

	/** Returns the mutable selected-block outline mesh. */
	BlockSelectionCube& OpenGLRenderView::MutableSelectionCube() noexcept { return selection_cube_; }

	/** Returns the mutable ground/debug plane renderer. */
	Plane& OpenGLRenderView::MutablePlane() noexcept { return plane_; }

	/** Returns the model importer library used by demos and tools. */
	ve::assets::ModelAssetLibrary& OpenGLRenderView::MutableModelAssets() noexcept { return model_asset_library_; }

	/** Returns the HUD renderer used for 2D overlays. */
	ve::ui::HudRenderer& OpenGLRenderView::MutableHudRenderer() noexcept { return hud_renderer_; }
}
