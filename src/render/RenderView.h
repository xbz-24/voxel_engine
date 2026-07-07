#pragma once

#include "RenderApi.h"

class BlockSelectionCube;
class Plane;
class SkyBox;

namespace ve::rendering
{
	class GraphicsFacade;
}

namespace ve::ui
{
	class HudRenderer;
}

namespace ve::engine
{
	/** Backend-neutral view contract consumed by the engine runtime. */
	class RenderView
	{
	public:
		/** Releases resources through the concrete render-view implementation. */
		virtual ~RenderView() = default;

		/** @return Graphics API represented by this view. */
		[[nodiscard]] virtual ve::rendering::GraphicsApi Api() const noexcept = 0;

		/** @return High-level primitive drawing API, or null when the backend has no executor yet. */
		[[nodiscard]] virtual ve::rendering::GraphicsFacade* Graphics() noexcept;

		/** @return Read-only primitive drawing API, or null when unavailable. */
		[[nodiscard]] virtual const ve::rendering::GraphicsFacade* Graphics() const noexcept;

		/** @return Skybox renderer capability, or null when unavailable. */
		[[nodiscard]] virtual SkyBox* Skybox() noexcept;

		/** @return Ground/debug plane renderer capability, or null when unavailable. */
		[[nodiscard]] virtual Plane* GroundPlane() noexcept;

		/** @return Selected-block renderer capability, or null when unavailable. */
		[[nodiscard]] virtual BlockSelectionCube* SelectionCube() noexcept;

		/** @return HUD renderer capability, or null when unavailable. */
		[[nodiscard]] virtual ve::ui::HudRenderer* Hud() noexcept;

		/** Renders an optional backend-owned sky/cloud layer before world geometry. */
		virtual void RenderCloudLayer();

		/** Releases optional cached native resources owned by the view. */
		virtual void ReleaseCachedResources();
	};
}
