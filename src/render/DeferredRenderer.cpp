#include "DeferredRenderer.h"

namespace
{
	/// Creates a 2D color target for a framebuffer.
	GLuint CreateColorTarget(int width, int height, GLenum internal_format)
	{
		GLuint texture = 0;
		glGenTextures(1, &texture);
		glBindTexture(GL_TEXTURE_2D, texture);
		glTexImage2D(GL_TEXTURE_2D, 0, static_cast<GLint>(internal_format), width, height, 0, GL_RGBA, GL_FLOAT, nullptr);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		return texture;
	}
}

namespace ve::rendering
{
	DeferredRenderer::~DeferredRenderer()
	{
		Release();
	}

	bool DeferredRenderer::Initialize(const DeferredFramebufferSpecification& specification)
	{
		Release();
		width_ = static_cast<int>(specification.width);
		height_ = static_cast<int>(specification.height);
		glGenFramebuffers(1, &framebuffer_);
		glBindFramebuffer(GL_FRAMEBUFFER, framebuffer_);
		albedo_texture_ = CreateColorTarget(width_, height_, GL_RGBA8);
		normal_texture_ = CreateColorTarget(width_, height_, GL_RGBA16F);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, albedo_texture_, 0);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, normal_texture_, 0);
		glGenTextures(1, &depth_texture_);
		glBindTexture(GL_TEXTURE_2D, depth_texture_);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT24, width_, height_, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depth_texture_, 0);
		const GLenum attachments[2] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1 };
		glDrawBuffers(2, attachments);
		const bool is_complete = glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE;
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		return is_complete;
	}

	void DeferredRenderer::BeginGeometryPass() const
	{
		glBindFramebuffer(GL_FRAMEBUFFER, framebuffer_);
		glViewport(0, 0, width_, height_);
	}

	void DeferredRenderer::EndGeometryPass() const
	{
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	void DeferredRenderer::Release()
	{
		if (albedo_texture_ != 0) glDeleteTextures(1, &albedo_texture_);
		if (normal_texture_ != 0) glDeleteTextures(1, &normal_texture_);
		if (depth_texture_ != 0) glDeleteTextures(1, &depth_texture_);
		if (framebuffer_ != 0) glDeleteFramebuffers(1, &framebuffer_);
		albedo_texture_ = normal_texture_ = depth_texture_ = framebuffer_ = 0;
	}

	bool DeferredRenderer::IsInitialized() const noexcept
	{
		return framebuffer_ != 0;
	}
}
