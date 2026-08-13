#pragma once

namespace ve::engine
{
	class RenderView;

	/** Backend-neutral engine services used by a runtime render driver. */
	class RuntimeRenderHost
	{
	public:
		virtual ~RuntimeRenderHost() = default;
		virtual void UpdateViewportProjection() = 0;
		virtual void RenderWorld(RenderView& view) = 0;
		virtual void RenderHud(RenderView& view) = 0;
	};
}
