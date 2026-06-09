#include "VoxelConeTracing.h"

namespace ve::rendering
{
	/// Releases the radiance volume.
	VoxelConeTracing::~VoxelConeTracing()
	{
		Release();
	}

	/// Creates a 3D radiance volume used by voxel cone tracing.
	bool VoxelConeTracing::Initialize(const VoxelConeTracingSettings& settings)
	{
		Release();
		resolution_ = settings.resolution;
		world_extent_ = settings.world_extent;
		glGenTextures(1, &radiance_texture_);
		glBindTexture(GL_TEXTURE_3D, radiance_texture_);
		glTexImage3D(GL_TEXTURE_3D, 0, GL_RGBA16F, resolution_, resolution_, resolution_, 0, GL_RGBA, GL_FLOAT, nullptr);
		glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
		glGenerateMipmap(GL_TEXTURE_3D);
		return radiance_texture_ != 0;
	}

	/// Binds the radiance texture for cone tracing shaders.
	void VoxelConeTracing::BindRadianceTexture(GLuint texture_unit) const
	{
		glActiveTexture(GL_TEXTURE0 + texture_unit);
		glBindTexture(GL_TEXTURE_3D, radiance_texture_);
	}

	/// Binds the radiance texture as an image for voxelization/lighting shaders.
	void VoxelConeTracing::BindRadianceImage(GLuint image_unit) const
	{
		glBindImageTexture(image_unit, radiance_texture_, 0, GL_TRUE, 0, GL_READ_WRITE, GL_RGBA16F);
	}

	/// Releases the 3D radiance texture.
	void VoxelConeTracing::Release()
	{
		if (radiance_texture_ != 0) glDeleteTextures(1, &radiance_texture_);
		radiance_texture_ = 0;
	}

	/// Reports whether the radiance volume exists.
	bool VoxelConeTracing::IsInitialized() const noexcept { return radiance_texture_ != 0; }

	/// Returns the radiance volume OpenGL texture id.
	GLuint VoxelConeTracing::RadianceTexture() const noexcept { return radiance_texture_; }
}
