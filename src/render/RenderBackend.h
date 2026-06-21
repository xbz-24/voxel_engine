#pragma once

#include "ComputeDispatcher.h"
#include "RenderApi.h"
#include "RenderMesh.h"

#include <memory>

namespace ve::rendering
{
	/** Common object-oriented contract implemented by every graphics backend. */
	class RenderBackend
	{
	public:
		virtual ~RenderBackend() = default;

		/** @return Graphics API represented by this backend. */
		[[nodiscard]] virtual GraphicsApi Api() const noexcept = 0;

		/** @return Runtime feature flags and availability for this backend. */
		[[nodiscard]] virtual RenderBackendCapabilities Capabilities() const noexcept = 0;

		/** @return Migration status for backend-neutral engine integration. */
		[[nodiscard]] virtual RenderBackendMigrationStatus MigrationStatus() const noexcept;

		/** @return Human-readable backend name for logs and UI. */
		[[nodiscard]] const char* Name() const noexcept;

		/** @return Backend-owned mesh resource, or null when the backend cannot create generic meshes yet. */
		[[nodiscard]] virtual std::unique_ptr<RenderMesh> CreateMeshResource() const;

		/** @return Backend compute dispatcher, or null when compute dispatch is unavailable. */
		[[nodiscard]] virtual const ComputeDispatcher* Compute() const noexcept;
	};
}
