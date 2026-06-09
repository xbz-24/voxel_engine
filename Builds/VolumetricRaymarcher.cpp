#include "VolumetricRaymarcher.h"

namespace ve::rendering
{
	/// Creates a raymarching pass wrapper.
	VolumetricRaymarcher::VolumetricRaymarcher(GLuint shader_program) noexcept
		: shader_program_(shader_program)
	{
	}

	/// Sets the fullscreen raymarching shader program.
	void VolumetricRaymarcher::SetProgram(GLuint shader_program) noexcept
	{
		shader_program_ = shader_program;
	}

	/// Sets ray step count and density parameters.
	void VolumetricRaymarcher::SetSettings(RaymarchingSettings settings) noexcept
	{
		settings_ = settings;
	}

	/// Binds the raymarching program before drawing the fullscreen pass.
	void VolumetricRaymarcher::BeginPass() const
	{
		if (shader_program_ != 0) glUseProgram(shader_program_);
	}

	/// Unbinds the raymarching program after drawing the fullscreen pass.
	void VolumetricRaymarcher::EndPass() const
	{
		if (shader_program_ != 0) glUseProgram(0);
	}

	/// Returns ray step count and density parameters.
	const RaymarchingSettings& VolumetricRaymarcher::Settings() const noexcept
	{
		return settings_;
	}
}
