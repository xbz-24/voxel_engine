#include "ComputeTerrainGenerator.h"

namespace ve::world::generation
{
	/// Creates a compute terrain dispatcher.
	ComputeTerrainGenerator::ComputeTerrainGenerator(GLuint compute_program) noexcept
		: compute_program_(compute_program)
	{
	}

	/// Sets the OpenGL compute shader program used for terrain generation.
	void ComputeTerrainGenerator::SetProgram(GLuint compute_program) noexcept
	{
		compute_program_ = compute_program;
	}

	/// Reports whether compute shaders are supported by the current OpenGL context.
	bool ComputeTerrainGenerator::IsSupported() const noexcept
	{
		return GLEW_VERSION_4_3 || GLEW_ARB_compute_shader;
	}

	/// Reports whether a compute program has been assigned.
	bool ComputeTerrainGenerator::HasProgram() const noexcept
	{
		return compute_program_ != 0;
	}

	/// Dispatches terrain generation work groups and flushes shader writes.
	void ComputeTerrainGenerator::Dispatch(GLuint groups_x, GLuint groups_y, GLuint groups_z) const
	{
		if (!IsSupported() || !HasProgram()) return;
		glUseProgram(compute_program_);
		glDispatchCompute(groups_x, groups_y, groups_z);
		glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT | GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
		glUseProgram(0);
	}
}
