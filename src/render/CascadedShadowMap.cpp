#include "CascadedShadowMap.h"

namespace ve::rendering
{
	CascadedShadowMap::~CascadedShadowMap()
	{
		Release();
	}

	bool CascadedShadowMap::Initialize(std::size_t cascade_count, int resolution)
	{
		Release();
		resolution_ = resolution;
		depth_textures_.resize(cascade_count, 0);
		glGenFramebuffers(1, &framebuffer_);
		glGenTextures(static_cast<GLsizei>(depth_textures_.size()), depth_textures_.data());
		for (GLuint depth_texture : depth_textures_)
		{
			glBindTexture(GL_TEXTURE_2D, depth_texture);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT24, resolution_, resolution_, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
		}
		return framebuffer_ != 0 && !depth_textures_.empty();
	}

	void CascadedShadowMap::BeginCascade(std::size_t cascade_index) const
	{
		if (cascade_index >= depth_textures_.size()) return;
		glViewport(0, 0, resolution_, resolution_);
		glBindFramebuffer(GL_FRAMEBUFFER, framebuffer_);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depth_textures_[cascade_index], 0);
		glDrawBuffer(GL_NONE);
		glReadBuffer(GL_NONE);
		glClear(GL_DEPTH_BUFFER_BIT);
	}

	void CascadedShadowMap::EndCascade() const
	{
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	void CascadedShadowMap::Release()
	{
		if (!depth_textures_.empty()) glDeleteTextures(static_cast<GLsizei>(depth_textures_.size()), depth_textures_.data());
		if (framebuffer_ != 0) glDeleteFramebuffers(1, &framebuffer_);
		depth_textures_.clear();
		framebuffer_ = 0;
	}

	std::size_t CascadedShadowMap::CascadeCount() const noexcept
	{
		return depth_textures_.size();
	}
}
