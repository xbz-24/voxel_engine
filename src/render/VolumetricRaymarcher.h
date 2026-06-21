#pragma once

#include <GL/glew.h>

namespace ve::rendering
{
	struct RaymarchingSettings
	{
		// TODO: Route volumetric passes through RenderCommandList so GL shader ids do not leak into high-level effects.
		int step_count = 64;
		float max_distance = 96.0f;
		float density = 0.035f;
		float anisotropy = 0.25f;
	};

	class VolumetricRaymarcher
	{
	public:
		/** @param shader_program Fullscreen raymarching shader program. */
		explicit VolumetricRaymarcher(GLuint shader_program = 0) noexcept;

		/** @param shader_program Fullscreen raymarching shader program. */
		void SetProgram(GLuint shader_program) noexcept;

		/** @param settings Ray step count and density parameters. */
		void SetSettings(RaymarchingSettings settings) noexcept;

		/** Binds the raymarching program before drawing the fullscreen pass. */
		void BeginPass() const;

		/** Unbinds the raymarching program after drawing the fullscreen pass. */
		void EndPass() const;

		/** @return Ray step count and density parameters. */
		[[nodiscard]] const RaymarchingSettings& Settings() const noexcept;

	private:
		GLuint shader_program_ = 0;
		RaymarchingSettings settings_{};
	};
}
