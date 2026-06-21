#include "MonteCarloPathTracer.h"
#include "PathTracingIntersection.h"

#include <glm/geometric.hpp>

namespace ve::rendering
{
	namespace
	{
		/** @param scene Scene to query. @param ray Ray to trace. @param hit Receives closest hit. @return True when hit. */
		bool FindClosestHit(const PathTracingScene& scene, const PathRay& ray, PathHit& hit);
	}

	/** Integrates radiance by bouncing through diffuse surfaces. */
	glm::vec3 MonteCarloPathTracer::TraceRay(const PathTracingScene& scene, PathRay ray, std::uint32_t seed) const
	{
		PathTracingRandom random(seed);
		glm::vec3 radiance{ 0.0f };
		glm::vec3 throughput{ 1.0f };
		for (int bounce = 0; bounce < settings_.max_bounces; ++bounce)
		{
			PathHit hit{};
			if (!FindClosestHit(scene, ray, hit))
			{
				radiance += throughput * glm::vec3{ 0.02f, 0.04f, 0.08f };
				break;
			}
			radiance += throughput * hit.material.emission;
			throughput *= hit.material.albedo;
			ray.origin = ray.origin + ray.direction * hit.distance + hit.normal * 0.001f;
			ray.direction = SampleHemisphere(random, hit.normal);
		}
		return radiance;
	}

	namespace
	{
		/** Finds the closest sphere hit for a ray. */
		bool FindClosestHit(const PathTracingScene& scene, const PathRay& ray, PathHit& hit)
		{
			bool has_hit = false;
			for (const PathTracingSphere& sphere : scene.Spheres())
			{
				has_hit = IntersectSphere(sphere, ray, hit) || has_hit;
			}
			return has_hit;
		}
	}
}
