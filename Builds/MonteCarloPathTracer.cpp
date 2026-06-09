#include "MonteCarloPathTracer.h"

#include <glm/geometric.hpp>

#include <algorithm>

namespace ve::rendering
{
	/** Stores path tracer settings with safe defaults. */
	MonteCarloPathTracer::MonteCarloPathTracer(MonteCarloPathTracerSettings settings)
		: settings_{ std::max(settings.max_bounces, 1) }
	{
	}

	/** Samples a diffuse hemisphere around a normal. */
	glm::vec3 MonteCarloPathTracer::SampleHemisphere(PathTracingRandom& random, const glm::vec3& normal) const
	{
		const glm::vec3 sample{ random.NextFloat() * 2.0f - 1.0f, random.NextFloat() * 2.0f - 1.0f, random.NextFloat() * 2.0f - 1.0f };
		const glm::vec3 direction = glm::normalize(sample + normal);
		return glm::dot(direction, normal) < 0.0f ? -direction : direction;
	}
}
