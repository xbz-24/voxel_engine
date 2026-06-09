#pragma once

#include "Render2D.h"

#include <glm/glm.hpp>

namespace ve::rendering
{
	struct CubeRequest
	{
		glm::vec3 center{ 0.0f };
		glm::vec3 size{ 1.0f };
		ColorRgba color;
	};

	/**
	 * Draws a filled axis-aligned cube using the fixed-function pipeline.
	 *
	 * @param request Cube center, size, and color.
	 */
	void DrawSolidCube(const CubeRequest& request);

	/**
	 * Draws an outlined axis-aligned cube using the fixed-function pipeline.
	 *
	 * @param request Cube center, size, and color.
	 * @param line_width Outline width in pixels.
	 */
	void DrawWireCube(const CubeRequest& request, float line_width);
}
