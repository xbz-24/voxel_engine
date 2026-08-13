#pragma once

#include "voxel/MaterialTypes.h"

namespace voxel
{
	/** Scene-wide environmental lighting and fog request. */
	struct Environment
	{
		Color ambient{ 0.35f, 0.38f, 0.44f, 1.0f };
		Color fog{ 0.56f, 0.68f, 0.84f, 1.0f };
		float fog_density = 0.0f;
		float time_of_day = 12.0f;

		Environment& Ambient(Color value) noexcept;
		Environment& Fog(Color value, float density) noexcept;
		Environment& TimeOfDay(float hour) noexcept;
	};
}
