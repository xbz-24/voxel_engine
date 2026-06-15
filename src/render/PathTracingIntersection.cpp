#include "PathTracingIntersection.h"

#include <glm/geometric.hpp>

#include <cmath>

namespace ve::rendering
{
	/** Tests a ray against a sphere and keeps only the closest positive hit. */
	bool IntersectSphere(const PathTracingSphere& sphere, const PathRay& ray, PathHit& hit)
	{
		const glm::vec3 offset = ray.origin - sphere.center;
		const float b = glm::dot(offset, ray.direction);
		const float c = glm::dot(offset, offset) - sphere.radius * sphere.radius;
		const float discriminant = b * b - c;
		if (discriminant < 0.0f) return false;
		const float distance = -b - std::sqrt(discriminant);
		if (distance <= 0.001f || (hit.distance > 0.0f && distance >= hit.distance)) return false;
		hit.distance = distance;
		hit.normal = glm::normalize(ray.origin + ray.direction * distance - sphere.center);
		hit.material = sphere.material;
		return true;
	}
}
