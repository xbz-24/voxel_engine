#pragma once

#include "PathTracingRandom.h"
#include "PathTracingScene.h"

namespace ve::rendering
{
	struct MonteCarloPathTracerSettings
	{
		int max_bounces = 4;
	};

	/** CPU reference path tracer using Monte Carlo hemisphere sampling. */
	class MonteCarloPathTracer
	{
	public:
		/** @param settings Bounce count and integrator controls. */
		explicit MonteCarloPathTracer(MonteCarloPathTracerSettings settings = {});

		/** @param scene Scene to trace. @param ray Camera ray. @param seed Random seed. @return Estimated radiance. */
		[[nodiscard]] glm::vec3 TraceRay(const PathTracingScene& scene, PathRay ray, std::uint32_t seed) const;

	private:
		/** @param random Random stream. @param normal Surface normal. @return Cosine-ish hemisphere direction. */
		[[nodiscard]] glm::vec3 SampleHemisphere(PathTracingRandom& random, const glm::vec3& normal) const;

		MonteCarloPathTracerSettings settings_;
	};
}
