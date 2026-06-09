#pragma once

#include "AmbientOcclusionPass.h"
#include "CascadedShadowMap.h"
#include "DeferredRenderer.h"

#include <cstddef>

namespace ve::rendering
{
	struct VoxelRenderPipelineSettings
	{
		int width = 1;
		int height = 1;
		std::size_t shadow_cascade_count = 4;
		int shadow_resolution = 2048;
	};

	class VoxelRenderPipeline
	{
	public:
		/** Releases all render pass resources. */
		~VoxelRenderPipeline();

		/** @param settings Framebuffer and shadow-map settings. @return True when every pass initializes. */
		[[nodiscard]] bool Initialize(const VoxelRenderPipelineSettings& settings);

		/** Releases all owned pass resources. */
		void Release();

		/** @return Deferred G-buffer pass. */
		[[nodiscard]] DeferredRenderer& Deferred() noexcept;

		/** @return Screen-space ambient occlusion pass. */
		[[nodiscard]] AmbientOcclusionPass& AmbientOcclusion() noexcept;

		/** @return Cascaded shadow map pass. */
		[[nodiscard]] CascadedShadowMap& Shadows() noexcept;

	private:
		DeferredRenderer deferred_;
		AmbientOcclusionPass ambient_occlusion_;
		CascadedShadowMap shadows_;
	};
}
