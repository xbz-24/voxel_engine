#pragma once

#include "RenderBackend.h"

namespace ve::rendering
{
	/** Placeholder backend descriptor for the future DirectX 12 renderer path. */
	class DirectX12Backend final : public RenderBackend
	{
	public:
		/** @return Conservative capabilities until the DirectX device path exists. */
		[[nodiscard]] RenderBackendCapabilities Capabilities() const noexcept override;

		/** @return Current backend-neutral migration coverage. */
		[[nodiscard]] RenderBackendMigrationStatus MigrationStatus() const noexcept override;

		/** @return API represented by this backend. */
		[[nodiscard]] GraphicsApi Api() const noexcept override;
	};
}
