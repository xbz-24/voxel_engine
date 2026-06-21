#pragma once

#include "AssetPaths.h"
#include "BlockSelectionCube.h"
#include "GraphicsFacade.h"
#include "HudRenderer.h"
#include "ModelAssetLibrary.h"
#include "OpenGLRenderCommandExecutor.h"
#include "Plane.h"
#include "RenderView.h"
#include "SkyBox.h"

namespace ve::engine
{
	/** OpenGL compatibility adapter for the current voxel world renderer. */
	class OpenGLRenderView final : public RenderView
	{
	public:
		/** @param asset_paths Resolved asset paths used by OpenGL render resources. */
		explicit OpenGLRenderView(const ve::assets::AssetPaths& asset_paths);

		/** @return API represented by this compatibility view. */
		[[nodiscard]] ve::rendering::GraphicsApi Api() const noexcept override;

		/** @return High-level primitive drawing API backed by OpenGL commands. */
		[[nodiscard]] ve::rendering::GraphicsFacade* Graphics() noexcept override;

		/** @return Read-only high-level primitive drawing API. */
		[[nodiscard]] const ve::rendering::GraphicsFacade* Graphics() const noexcept override;

		/** @return Skybox render service. */
		[[nodiscard]] SkyBox* Skybox() noexcept;

		/** @return Ground/debug plane render service. */
		[[nodiscard]] Plane* GroundPlane() noexcept;

		/** @return Selected-block render service. */
		[[nodiscard]] BlockSelectionCube* SelectionCube() noexcept;

		/** @return HUD render service. */
		[[nodiscard]] ve::ui::HudRenderer* Hud() noexcept;

		/** @return This object as the OpenGL compatibility adapter. */
		[[nodiscard]] OpenGLRenderView* AsOpenGLRenderView() noexcept override;

		/** @return This object as the read-only OpenGL compatibility adapter. */
		[[nodiscard]] const OpenGLRenderView* AsOpenGLRenderView() const noexcept override;

		/** @return Mutable skybox renderer. */
		[[nodiscard]] SkyBox& MutableSkyBox() noexcept;

		/** @return Mutable selected-block outline mesh. */
		[[nodiscard]] BlockSelectionCube& MutableSelectionCube() noexcept;

		/** @return Mutable ground/debug plane renderer. */
		[[nodiscard]] Plane& MutablePlane() noexcept;

		/** @return Model importer library used by demos and tools. */
		[[nodiscard]] ve::assets::ModelAssetLibrary& MutableModelAssets() noexcept;

		/** @return HUD renderer used for 2D overlays. */
		[[nodiscard]] ve::ui::HudRenderer& MutableHudRenderer() noexcept;

	private:
		SkyBox sky_box_;
		BlockSelectionCube selection_cube_;
		Plane plane_;
		ve::assets::ModelAssetLibrary model_asset_library_;
		ve::ui::HudRenderer hud_renderer_;
		ve::rendering::OpenGLRenderCommandExecutor primitive_command_executor_;
		ve::rendering::GraphicsFacade graphics_;
	};
}
