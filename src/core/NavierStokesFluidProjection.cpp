#include "NavierStokesFluidSimulation.h"

#include <algorithm>

namespace ve::simulation
{
	/** Computes negative divergence used by the pressure solve. */
	void NavierStokesFluidSimulation::ComputeDivergence()
	{
		std::fill(pressure_.begin(), pressure_.end(), 0.0f);
		for (int y = 0; y < current_.Height(); ++y)
		{
			for (int x = 0; x < current_.Width(); ++x)
			{
				const float horizontal = current_.VelocityAt(x + 1, y).x - current_.VelocityAt(x - 1, y).x;
				const float vertical = current_.VelocityAt(x, y + 1).y - current_.VelocityAt(x, y - 1).y;
				divergence_[Index(x, y)] = -0.5f * (horizontal + vertical);
			}
		}
	}

	/** Solves pressure with Jacobi iterations. */
	void NavierStokesFluidSimulation::SolvePressure()
	{
		ve::core::DynamicArray<float> next_pressure = pressure_;
		for (int iteration = 0; iteration < settings_.pressure_iterations; ++iteration)
		{
			for (int y = 0; y < current_.Height(); ++y)
			{
				for (int x = 0; x < current_.Width(); ++x)
				{
					const float neighbor_sum = PressureAt(x - 1, y) + PressureAt(x + 1, y) + PressureAt(x, y - 1) + PressureAt(x, y + 1);
					next_pressure[Index(x, y)] = (divergence_[Index(x, y)] + neighbor_sum) * 0.25f;
				}
			}
			pressure_.swap(next_pressure);
		}
	}

	/** Projects velocity by subtracting the pressure gradient. */
	void NavierStokesFluidSimulation::SubtractPressureGradient()
	{
		for (int y = 0; y < current_.Height(); ++y)
		{
			for (int x = 0; x < current_.Width(); ++x)
			{
				const glm::vec2 gradient{ PressureAt(x + 1, y) - PressureAt(x - 1, y), PressureAt(x, y + 1) - PressureAt(x, y - 1) };
				current_.SetVelocity(x, y, current_.VelocityAt(x, y) - gradient * 0.5f);
			}
		}
	}
}
