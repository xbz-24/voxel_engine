#pragma once

#include <glm/glm.hpp>

namespace ve::rendering
{
	struct PathRay
	{
		glm::vec3 origin{ 0.0f };
		glm::vec3 direction{ 0.0f, 0.0f, -1.0f };
	};

	struct PathTracingMaterial
	{
		glm::vec3 albedo{ 1.0f };
		glm::vec3 emission{ 0.0f };
	};

	struct PathTracingSphere
	{
		glm::vec3 center{ 0.0f };
		float radius = 1.0f;
		PathTracingMaterial material{};
	};
}
