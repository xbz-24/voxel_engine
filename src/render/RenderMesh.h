#pragma once

#include "GraphicsTypes.h"

#include <memory>

namespace ve::rendering
{
	/** Backend-owned mesh resource used by higher-level world/render code. */
	class RenderMesh
	{
	public:
		virtual ~RenderMesh() noexcept = default;

		/** @param description CPU mesh payload to upload into backend-owned memory. */
		virtual void Upload(const MeshDescription& description) = 0;

		/** Draws the uploaded mesh through the owning backend. */
		virtual void Draw() const = 0;

		/** Releases native backend resources. */
		virtual void Release() noexcept = 0;
	};

	/** @return Compatibility OpenGL mesh resource. */
	[[nodiscard]] std::unique_ptr<RenderMesh> CreateOpenGLRenderMesh();
}
