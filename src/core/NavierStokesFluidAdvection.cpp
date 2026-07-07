#include "NavierStokesFluidSimulation.h"

namespace ve::simulation
{
	/** Backtraces velocity through the previous velocity field. */
	void NavierStokesFluidSimulation::AdvectVelocity(float delta_seconds)
	{
		for (int cell_y = 0; cell_y < current_.Height(); ++cell_y)
		{
			for (int cell_x = 0; cell_x < current_.Width(); ++cell_x)
			{
				const glm::vec2 position{ static_cast<float>(cell_x), static_cast<float>(cell_y) };
				const glm::vec2 velocity = previous_.VelocityAt(cell_x, cell_y);
				const glm::vec2 backtraced_position = position - velocity * delta_seconds;
				current_.SetVelocity(
					cell_x,
					cell_y,
					previous_.SampleVelocity(backtraced_position) * settings_.velocity_damping);
			}
		}
	}

	/** Backtraces density through the projected velocity field. */
	void NavierStokesFluidSimulation::AdvectDensity(float delta_seconds)
	{
		for (int cell_y = 0; cell_y < current_.Height(); ++cell_y)
		{
			for (int cell_x = 0; cell_x < current_.Width(); ++cell_x)
			{
				const glm::vec2 position{ static_cast<float>(cell_x), static_cast<float>(cell_y) };
				const glm::vec2 velocity = current_.VelocityAt(cell_x, cell_y);
				const glm::vec2 backtraced_position = position - velocity * delta_seconds;
				current_.SetDensity(cell_x, cell_y, previous_.SampleDensity(backtraced_position));
			}
		}
	}
}
