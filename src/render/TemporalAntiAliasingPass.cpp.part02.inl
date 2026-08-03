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
