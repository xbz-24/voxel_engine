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
