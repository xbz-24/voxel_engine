#include "AmbientOcclusionPass.h"

#include <algorithm>
#include <cstdint>

namespace
{
	/// Converts dimensions to the unsigned frame graph metadata type.
	ve::core::Index ResourceExtent(int value) noexcept
	{
		return static_cast<ve::core::Index>(std::max(1, value));
	}

	/// Creates one single-channel texture used as the AO output.
	GLuint CreateOcclusionTexture(int width, int height)
	{
		GLuint texture = 0;
		glGenTextures(1, &texture);
		glBindTexture(GL_TEXTURE_2D, texture);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_R8, width, height, 0, GL_RED, GL_UNSIGNED_BYTE, nullptr);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		return texture;
	}
}

namespace ve::rendering
{
	/// Releases OpenGL resources owned by the AO pass.
	AmbientOcclusionPass::~AmbientOcclusionPass()
	{
		Release();
	}

	/// Initializes the output target used by a screen-space AO shader.
	bool AmbientOcclusionPass::Initialize(const AmbientOcclusionSpecification& specification)
	{
		Release();
		width_ = specification.width;
		height_ = specification.height;
		radius_ = specification.radius;
		strength_ = specification.strength;
		resources_ = specification.resources;
		if (resources_.depth_input.name.empty() || resources_.occlusion_output.name.empty())
		{
			resources_ = DescribeResources(width_, height_);
		}
		glGenFramebuffers(1, &framebuffer_);
		glBindFramebuffer(GL_FRAMEBUFFER, framebuffer_);
		occlusion_texture_ = CreateOcclusionTexture(width_, height_);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, occlusion_texture_, 0);
		const GLenum attachment = GL_COLOR_ATTACHMENT0;
		glDrawBuffers(1, &attachment);
		const bool is_complete = glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE;
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		return is_complete;
	}

	/// Describes AO frame graph resources without requiring an OpenGL context.
	AmbientOcclusionResources AmbientOcclusionPass::DescribeResources(int width, int height)
	{
		AmbientOcclusionResources resources{};
		resources.depth_input.name = "ao.depth";
		resources.depth_input.format = FrameGraphResourceFormat::Depth24Stencil8;
		resources.depth_input.width = ResourceExtent(width);
		resources.depth_input.height = ResourceExtent(height);
		resources.depth_input.lifetime = FrameGraphResourceLifetime::Imported;
		resources.depth_input.imported = true;

		resources.occlusion_output.name = "ao.occlusion";
		resources.occlusion_output.format = FrameGraphResourceFormat::R8;
		resources.occlusion_output.width = ResourceExtent(width);
		resources.occlusion_output.height = ResourceExtent(height);
		resources.occlusion_output.lifetime = FrameGraphResourceLifetime::Exported;
		resources.occlusion_output.exported = true;
		return resources;
	}

	/// Returns the current AO frame graph resource metadata.
	const AmbientOcclusionResources& AmbientOcclusionPass::Resources() const noexcept { return resources_; }

	/// Binds the AO framebuffer so the occlusion shader can write into it.
	void AmbientOcclusionPass::BeginPass() const
	{
		glBindFramebuffer(GL_FRAMEBUFFER, framebuffer_);
		glViewport(0, 0, width_, height_);
	}

	/// Unbinds the AO framebuffer after the shader pass.
	void AmbientOcclusionPass::EndPass() const { glBindFramebuffer(GL_FRAMEBUFFER, 0); }

	/// Releases framebuffer and texture resources.
	void AmbientOcclusionPass::Release()
	{
		if (occlusion_texture_ != 0) glDeleteTextures(1, &occlusion_texture_);
		if (framebuffer_ != 0) glDeleteFramebuffers(1, &framebuffer_);
		framebuffer_ = 0;
		occlusion_texture_ = 0;
	}

	/// Reports whether the AO framebuffer exists.
	bool AmbientOcclusionPass::IsInitialized() const noexcept { return framebuffer_ != 0; }

	/// Returns the texture containing the last ambient occlusion result.
	GLuint AmbientOcclusionPass::Texture() const noexcept { return occlusion_texture_; }
}
