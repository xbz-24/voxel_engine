#pragma once

#include "RenderBackend.h"

namespace ve::rendering
{
	/** Names the existing OpenGL renderer while Vulkan migration is still in progress. */
	class OpenGLCompatibilityBackend final : public RenderBackend
	{
	public:
		/** @return Feature set exposed by the compatibility renderer. */
		[[nodiscard]] RenderBackendCapabilities Capabilities() const noexcept override;

		/** @return API represented by this backend. */
		[[nodiscard]] GraphicsApi Api() const noexcept override;
	};
}
