#include "NavierStokesFluidSimulation.h"

#include <algorithm>

namespace ve::simulation
{
	/** Creates a fluid simulation and allocates solver fields. */
	NavierStokesFluidSimulation::NavierStokesFluidSimulation(const NavierStokesFluidSettings& settings)
		: settings_(settings), current_(settings.width, settings.height), previous_(settings.width, settings.height)
	{
		settings_.width = std::max(settings_.width, 1);
		settings_.height = std::max(settings_.height, 1);
		settings_.pressure_iterations = std::max(settings_.pressure_iterations, 1);
		const auto cell_count = static_cast<ve::core::Index>(settings_.width * settings_.height);
		pressure_.assign(cell_count, 0.0f);
		divergence_.assign(cell_count, 0.0f);
	}

	/** Adds density to the active field. */
	void NavierStokesFluidSimulation::AddDensity(int cell_x, int cell_y, float amount)
	{
		current_.AddDensity(cell_x, cell_y, amount);
	}

	/** Adds velocity to the active field. */
	void NavierStokesFluidSimulation::AddVelocity(int cell_x, int cell_y, const glm::vec2& velocity)
	{
		current_.AddVelocity(cell_x, cell_y, velocity);
	}

	/** Advances the stable-fluid solver. */
	void NavierStokesFluidSimulation::Step(float delta_seconds)
	{
		if (delta_seconds <= 0.0f) return;
		previous_ = current_;
		AdvectVelocity(delta_seconds);
		ComputeDivergence();
		SolvePressure();
		SubtractPressureGradient();
		previous_ = current_;
		AdvectDensity(delta_seconds);
	}

	/** Returns the active simulation grid. */
	const FluidGrid& NavierStokesFluidSimulation::Grid() const noexcept { return current_; }

	/** Returns a clamped pressure value. */
	float NavierStokesFluidSimulation::PressureAt(int cell_x, int cell_y) const noexcept
	{
		return pressure_[Index(cell_x, cell_y)];
	}

	/** Converts cell coordinates to a clamped flat index. */
	ve::core::Index NavierStokesFluidSimulation::Index(int cell_x, int cell_y) const noexcept
	{
		const int clamped_x = std::clamp(cell_x, 0, current_.Width() - 1);
		const int clamped_y = std::clamp(cell_y, 0, current_.Height() - 1);
		return static_cast<ve::core::Index>(clamped_y * current_.Width() + clamped_x);
	}
}
