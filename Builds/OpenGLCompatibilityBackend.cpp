#include "OpenGLCompatibilityBackend.h"

namespace ve::rendering
{
	/** Returns conservative capabilities for the legacy OpenGL path. */
	RenderBackendCapabilities OpenGLCompatibilityBackend::Capabilities() const noexcept
	{
		return { false, false, false, true };
	}

	/** Returns the compatibility API identifier. */
	GraphicsApi OpenGLCompatibilityBackend::Api() const noexcept
	{
		return GraphicsApi::OpenGLCompatibility;
	}
}
