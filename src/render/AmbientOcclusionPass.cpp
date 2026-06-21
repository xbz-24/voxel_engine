#include "AmbientOcclusionPass.h"

namespace
{
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
