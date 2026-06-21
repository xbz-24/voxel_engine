#include "TemporalAntiAliasingPass.h"

namespace
{
	/// Creates one floating-point history texture.
	GLuint CreateHistoryTexture(int width, int height)
	{
		GLuint texture = 0;
		glGenTextures(1, &texture);
		glBindTexture(GL_TEXTURE_2D, texture);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, width, height, 0, GL_RGBA, GL_FLOAT, nullptr);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		return texture;
	}
}

namespace ve::rendering
{
	/// Releases OpenGL history resources.
	TemporalAntiAliasingPass::~TemporalAntiAliasingPass() { Release(); }

	/// Initializes history targets used by a TAA resolve shader.
	bool TemporalAntiAliasingPass::Initialize(const TemporalAntiAliasingSettings& settings)
	{
		Release();
		width_ = settings.width;
		height_ = settings.height;
		history_blend_ = settings.history_blend;
		glGenFramebuffers(1, &framebuffer_);
		history_textures_[0] = CreateHistoryTexture(width_, height_);
		history_textures_[1] = CreateHistoryTexture(width_, height_);
		return framebuffer_ != 0 && history_textures_[0] != 0 && history_textures_[1] != 0;
	}

	/// Advances the history buffer and jitter sequence by one frame.
	void TemporalAntiAliasingPass::AdvanceFrame() noexcept { frame_index_++; }

	/// Releases history framebuffer and textures.
	void TemporalAntiAliasingPass::Release()
	{
		if (history_textures_[0] != 0) glDeleteTextures(1, &history_textures_[0]);
		if (history_textures_[1] != 0) glDeleteTextures(1, &history_textures_[1]);
		if (framebuffer_ != 0) glDeleteFramebuffers(1, &framebuffer_);
		history_textures_[0] = history_textures_[1] = framebuffer_ = 0;
	}
}
