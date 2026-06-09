#include "NavierStokesFluidSimulation.h"

namespace ve::simulation
{
	/** Backtraces velocity through the previous velocity field. */
	void NavierStokesFluidSimulation::AdvectVelocity(float delta_seconds)
	{
		for (int y = 0; y < current_.Height(); ++y)
		{
			for (int x = 0; x < current_.Width(); ++x)
			{
				const glm::vec2 position{ static_cast<float>(x), static_cast<float>(y) };
				const glm::vec2 velocity = previous_.VelocityAt(x, y);
				const glm::vec2 backtraced_position = position - velocity * delta_seconds;
				current_.SetVelocity(x, y, previous_.SampleVelocity(backtraced_position) * settings_.velocity_damping);
			}
		}
	}

	/** Backtraces density through the projected velocity field. */
	void NavierStokesFluidSimulation::AdvectDensity(float delta_seconds)
	{
		for (int y = 0; y < current_.Height(); ++y)
		{
			for (int x = 0; x < current_.Width(); ++x)
			{
				const glm::vec2 position{ static_cast<float>(x), static_cast<float>(y) };
				const glm::vec2 velocity = current_.VelocityAt(x, y);
				const glm::vec2 backtraced_position = position - velocity * delta_seconds;
				current_.SetDensity(x, y, previous_.SampleDensity(backtraced_position));
			}
		}
	}
}
