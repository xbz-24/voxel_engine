#include "TemporalAntiAliasingPass.h"

#include <algorithm>

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
		width_ = std::max(1, settings.width);
		height_ = std::max(1, settings.height);
		history_blend_ = settings.history_blend;
		history_cache_.resources = settings.history_resources.previous_history.name.empty() ||
				settings.history_resources.current_history.name.empty()
			? DescribeHistoryResources(width_, height_)
			: settings.history_resources;
		glGenFramebuffers(1, &history_cache_.framebuffer);
		history_cache_.textures[0] = CreateHistoryTexture(width_, height_);
		history_cache_.textures[1] = CreateHistoryTexture(width_, height_);
		return history_cache_.framebuffer != 0 && history_cache_.textures[0] != 0 && history_cache_.textures[1] != 0;
	}

	/// Advances the history buffer and jitter sequence by one frame.
	void TemporalAntiAliasingPass::AdvanceFrame() noexcept { frame_index_++; }

	/// Releases history framebuffer and textures.
	void TemporalAntiAliasingPass::Release()
	{
		for (GLuint& texture : history_cache_.textures)
		{
			if (texture != 0) glDeleteTextures(1, &texture);
			texture = 0;
		}
		if (history_cache_.framebuffer != 0) glDeleteFramebuffers(1, &history_cache_.framebuffer);
		history_cache_.framebuffer = 0;
	}
}
