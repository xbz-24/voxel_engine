#pragma once

#include "ComputeDispatcher.h"

#include <cstdint>

namespace ve::world::generation
{
	class ComputeTerrainGenerator
	{
	public:
		/**
		 * Creates a compute terrain dispatcher.
		 *
		 * @param compute_program Backend compute shader program, or empty until assigned.
		 * @param dispatcher Backend compute dispatcher, or null until compute is available.
		 */
		explicit ComputeTerrainGenerator(ve::rendering::ComputeProgramHandle compute_program = {},
			const ve::rendering::ComputeDispatcher* dispatcher = nullptr) noexcept;

		/** @param compute_program Backend compute shader program used for terrain generation. */
		void SetProgram(ve::rendering::ComputeProgramHandle compute_program) noexcept;

		/** @param dispatcher Backend compute dispatcher, or null when unavailable. */
		void SetDispatcher(const ve::rendering::ComputeDispatcher* dispatcher) noexcept;

		/** @return True when compute shaders are supported by the active backend. */
		[[nodiscard]] bool IsSupported() const noexcept;

		/** @return True when a compute program has been assigned. */
		[[nodiscard]] bool HasProgram() const noexcept;

		/** @param groups_x X work groups. @param groups_y Y work groups. @param groups_z Z work groups. */
		void Dispatch(std::uint32_t groups_x, std::uint32_t groups_y, std::uint32_t groups_z) const;

	private:
		ve::rendering::ComputeProgramHandle compute_program_;
		const ve::rendering::ComputeDispatcher* dispatcher_;
	};
}
