#include "DirectX12Backend.h"

#include "RenderBackendCatalog.h"

namespace ve::rendering
{
	/** Returns unavailable capabilities while the DirectX 12 backend is only declared. */
	RenderBackendCapabilities DirectX12Backend::Capabilities() const noexcept
	{
		return { true, true, true, false };
	}

	RenderBackendMigrationStatus DirectX12Backend::MigrationStatus() const noexcept
	{
		// TODO: Promote this backend only after device creation, swapchain, and smoke coverage exist.
		return RenderBackendCatalog::Find(Api()).migration;
	}

	/** Returns the DirectX 12 API identifier. */
	GraphicsApi DirectX12Backend::Api() const noexcept
	{
		return GraphicsApi::DirectX12;
	}
}
