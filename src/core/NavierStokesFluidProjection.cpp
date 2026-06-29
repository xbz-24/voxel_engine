#include "NavierStokesFluidSimulation.h"

#include <algorithm>

namespace ve::simulation
{
	/** Computes negative divergence used by the pressure solve. */
	void NavierStokesFluidSimulation::ComputeDivergence()
	{
		std::fill(pressure_.begin(), pressure_.end(), 0.0f);
		for (int cell_y = 0; cell_y < current_.Height(); ++cell_y)
		{
			for (int cell_x = 0; cell_x < current_.Width(); ++cell_x)
			{
				const float horizontal_change =
					current_.VelocityAt(cell_x + 1, cell_y).x -
					current_.VelocityAt(cell_x - 1, cell_y).x;
				const float vertical_change =
					current_.VelocityAt(cell_x, cell_y + 1).y -
					current_.VelocityAt(cell_x, cell_y - 1).y;
				divergence_[Index(cell_x, cell_y)] = -0.5f * (horizontal_change + vertical_change);
			}
		}
	}

	/** Solves pressure with Jacobi iterations. */
	void NavierStokesFluidSimulation::SolvePressure()
	{
		ve::core::DynamicArray<float> next_pressure = pressure_;
		for (int iteration = 0; iteration < settings_.pressure_iterations; ++iteration)
		{
			for (int cell_y = 0; cell_y < current_.Height(); ++cell_y)
			{
				for (int cell_x = 0; cell_x < current_.Width(); ++cell_x)
				{
					const float horizontal_pressure = PressureAt(cell_x - 1, cell_y) + PressureAt(cell_x + 1, cell_y);
					const float vertical_pressure = PressureAt(cell_x, cell_y - 1) + PressureAt(cell_x, cell_y + 1);
					const float neighbor_pressure = horizontal_pressure + vertical_pressure;
					next_pressure[Index(cell_x, cell_y)] =
						(divergence_[Index(cell_x, cell_y)] + neighbor_pressure) * 0.25f;
				}
			}
			pressure_.swap(next_pressure);
		}
	}

	/** Projects velocity by subtracting the pressure gradient. */
	void NavierStokesFluidSimulation::SubtractPressureGradient()
	{
		for (int cell_y = 0; cell_y < current_.Height(); ++cell_y)
		{
			for (int cell_x = 0; cell_x < current_.Width(); ++cell_x)
			{
				const glm::vec2 pressure_gradient{
					PressureAt(cell_x + 1, cell_y) - PressureAt(cell_x - 1, cell_y),
					PressureAt(cell_x, cell_y + 1) - PressureAt(cell_x, cell_y - 1)
				};
				current_.SetVelocity(
					cell_x,
					cell_y,
					current_.VelocityAt(cell_x, cell_y) - pressure_gradient * 0.5f);
			}
		}
	}
}
