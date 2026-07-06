#pragma once

#include "FrameGraphResource.h"

#include <GL/glew.h>

namespace ve::rendering
{
	struct AmbientOcclusionResources
	{
		FrameGraphResourceDescriptor depth_input{};
		FrameGraphResourceDescriptor occlusion_output{};
	};

	struct AmbientOcclusionSpecification
	{
		int width = 1;
		int height = 1;
		float radius = 0.75f;
		float strength = 1.0f;
		AmbientOcclusionResources resources{};
	};

	class AmbientOcclusionPass
	{
	public:
		/** Releases OpenGL resources owned by the AO pass. */
		~AmbientOcclusionPass();

		/** @param specification Output size and AO tuning values. @return True when the target is complete. */
		[[nodiscard]] bool Initialize(const AmbientOcclusionSpecification& specification);

		/** @param width Target width in pixels. @param height Target height in pixels. @return Graph resources used by AO. */
		[[nodiscard]] static AmbientOcclusionResources DescribeResources(int width, int height);

		/** @return Frame graph resource metadata used by the current AO target. */
		[[nodiscard]] const AmbientOcclusionResources& Resources() const noexcept;

		/** Binds the AO framebuffer so the occlusion shader can write into it. */
		void BeginPass() const;

		/** Unbinds the AO framebuffer after the shader pass. */
		void EndPass() const;

		/** Releases framebuffer and texture resources. */
		void Release();

		/** @return True when the AO framebuffer exists. */
		[[nodiscard]] bool IsInitialized() const noexcept;

		/** @return Texture containing the last ambient occlusion result. */
		[[nodiscard]] GLuint Texture() const noexcept;

	private:
		GLuint framebuffer_ = 0;
		GLuint occlusion_texture_ = 0;
		int width_ = 0;
		int height_ = 0;
		float radius_ = 0.75f;
		float strength_ = 1.0f;
		AmbientOcclusionResources resources_{};
	};
}
