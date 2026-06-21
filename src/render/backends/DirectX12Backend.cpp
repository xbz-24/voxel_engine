#include "DirectX12Backend.h"

namespace ve::rendering
{
	/** Returns unavailable capabilities while the DirectX 12 backend is only declared. */
	RenderBackendCapabilities DirectX12Backend::Capabilities() const noexcept
	{
		return { true, true, true, false };
	}

	/** Returns the DirectX 12 API identifier. */
	GraphicsApi DirectX12Backend::Api() const noexcept
	{
		return GraphicsApi::DirectX12;
	}
}
