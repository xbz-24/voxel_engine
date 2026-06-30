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

		/** @return OpenGL-backed generic mesh resource. */
		[[nodiscard]] std::unique_ptr<RenderMesh> CreateMeshResource() const override;

		/** @return OpenGL compute dispatcher. */
		[[nodiscard]] const ComputeDispatcher* Compute() const noexcept override;

	private:
		class OpenGLComputeDispatcher final : public ComputeDispatcher
		{
		public:
			[[nodiscard]] bool IsSupported() const noexcept override;
			void Dispatch(ComputeProgramHandle program, ComputeDispatchSize size) const override;
		};

		OpenGLComputeDispatcher compute_dispatcher_;
	};
}
