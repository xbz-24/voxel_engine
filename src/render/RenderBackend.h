#pragma once

#include "ComputeDispatcher.h"
#include "RenderApi.h"
#include "RenderMesh.h"
#include "RenderResources.h"

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

		/** @return Backend-owned texture resource, or null when textures are not supported yet. */
		[[nodiscard]] virtual RenderTexturePtr CreateTextureResource(const RenderTextureDescriptor& descriptor) const;

		/** @return Backend-owned uploaded texture resource, or null when uploads are not supported yet. */
		[[nodiscard]] virtual RenderTexturePtr CreateTextureResource(const RenderTextureUpload& upload) const;

		/** @return Backend-owned material resource, or null when materials are not supported yet. */
		[[nodiscard]] virtual RenderMaterialPtr CreateMaterialResource(const RenderMaterialDescriptor& descriptor) const;

		/** @return Backend-owned render target, or null when render targets are not supported yet. */
		[[nodiscard]] virtual RenderTargetPtr CreateRenderTarget(const RenderTargetDescriptor& descriptor) const;

		/** @return Backend-owned scene instance, or null when scene instances are not supported yet. */
		[[nodiscard]] virtual RenderSceneInstancePtr CreateSceneInstance(const RenderSceneInstanceDescriptor& descriptor) const;

		/** Called before a frame is recorded or submitted. Backends that do not need shared lifecycle hooks may ignore it. */
		virtual void BeginFrame();

		/** Called after a frame is recorded or submitted. Backends that do not need shared lifecycle hooks may ignore it. */
		virtual void EndFrame();

		/** @return Backend compute dispatcher, or null when compute dispatch is unavailable. */
		[[nodiscard]] virtual const ComputeDispatcher* Compute() const noexcept;
	};
}
