#pragma once

#include <GL/glew.h>

namespace ve::rendering
{
	struct VoxelConeTracingSettings
	{
		// TODO: Replace OpenGL texture/image binding calls with a backend-owned voxel lighting volume resource.
		int resolution = 128;
		float world_extent = 256.0f;
	};

	class VoxelConeTracing
	{
	public:
		~VoxelConeTracing();

		/** @param settings Volume resolution and world coverage. @return True when created. */
		[[nodiscard]] bool Initialize(const VoxelConeTracingSettings& settings);

		/** @param texture_unit Texture unit used by cone tracing shaders. */
		void BindRadianceTexture(GLuint texture_unit) const;

		/** @param image_unit Image unit used by voxelization/lighting shaders. */
		void BindRadianceImage(GLuint image_unit) const;

		/** Releases the 3D radiance texture. */
		void Release();

		/** @return True when the radiance volume exists. */
		[[nodiscard]] bool IsInitialized() const noexcept;

		/** @return Radiance volume OpenGL texture id. */
		[[nodiscard]] GLuint RadianceTexture() const noexcept;

	private:
		GLuint radiance_texture_ = 0;
		int resolution_ = 0;
		float world_extent_ = 0.0f;
	};
}
