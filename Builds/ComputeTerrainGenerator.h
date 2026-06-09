#pragma once

#include <GL/glew.h>

namespace ve::world::generation
{
	class ComputeTerrainGenerator
	{
	public:
		/**
		 * Creates a compute terrain dispatcher.
		 *
		 * @param compute_program OpenGL compute shader program, or zero until assigned.
		 */
		explicit ComputeTerrainGenerator(GLuint compute_program = 0) noexcept;

		/** @param compute_program OpenGL compute shader program used for terrain generation. */
		void SetProgram(GLuint compute_program) noexcept;

		/** @return True when compute shaders are supported by the current OpenGL context. */
		[[nodiscard]] bool IsSupported() const noexcept;

		/** @return True when a compute program has been assigned. */
		[[nodiscard]] bool HasProgram() const noexcept;

		/** @param groups_x X work groups. @param groups_y Y work groups. @param groups_z Z work groups. */
		void Dispatch(GLuint groups_x, GLuint groups_y, GLuint groups_z) const;

	private:
		GLuint compute_program_ = 0;
	};
}
