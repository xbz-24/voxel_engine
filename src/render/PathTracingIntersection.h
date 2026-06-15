#pragma once

#include "PathTracingTypes.h"

namespace ve::rendering
{
	struct PathHit
	{
		float distance = 0.0f;
		glm::vec3 normal{ 0.0f };
		PathTracingMaterial material{};
	};

	/** @param sphere Sphere to test. @param ray Ray in world space. @param hit Receives closest hit. @return True when hit. */
	[[nodiscard]] bool IntersectSphere(const PathTracingSphere& sphere, const PathRay& ray, PathHit& hit);
}
