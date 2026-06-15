#pragma once

#include "FluidGrid.h"

namespace ve::simulation
{
	struct NavierStokesFluidSettings
	{
		int width = 64;
		int height = 64;
		int pressure_iterations = 12;
		float velocity_damping = 0.995f;
	};

	/** Real-time 2D stable-fluid simulation for gameplay and visual effects. */
	class NavierStokesFluidSimulation
	{
	public:
		/** @param settings Grid size and solver quality settings. */
		explicit NavierStokesFluidSimulation(const NavierStokesFluidSettings& settings = {});

		/** @param x Cell X. @param y Cell Y. @param amount Density source amount. */
		void AddDensity(int x, int y, float amount);

		/** @param x Cell X. @param y Cell Y. @param velocity Velocity source amount. */
		void AddVelocity(int x, int y, const glm::vec2& velocity);

		/** @param delta_seconds Time step in seconds. */
		void Step(float delta_seconds);

		/** @return Read-only simulation grid. */
		[[nodiscard]] const FluidGrid& Grid() const noexcept;

	private:
		/** @param delta_seconds Time step used to backtrace density. */
		void AdvectDensity(float delta_seconds);
		/** @param delta_seconds Time step used to backtrace velocity. */
		void AdvectVelocity(float delta_seconds);
		/** Computes divergence from the current velocity field. */
		void ComputeDivergence();
		/** Solves pressure using Jacobi iterations. */
		void SolvePressure();
		/** Subtracts pressure gradient from velocity. */
		void SubtractPressureGradient();
		/** @param x Cell X. @param y Cell Y. @return Clamped pressure value. */
		[[nodiscard]] float PressureAt(int x, int y) const noexcept;
		/** @param x Cell X. @param y Cell Y. @return Clamped flat index. */
		[[nodiscard]] ve::core::Index Index(int x, int y) const noexcept;

		NavierStokesFluidSettings settings_;
		FluidGrid current_;
		FluidGrid previous_;
		ve::core::DynamicArray<float> pressure_;
		ve::core::DynamicArray<float> divergence_;
	};
}
