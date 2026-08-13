#pragma once

#include "voxel/Camera.h"
#include "voxel/MaterialTypes.h"

namespace voxel
{
	/** Light shape requested by authored scene data. */
	enum class LightKind
	{
		Directional,
		Point,
		Spot
	};

	/** Unit used to interpret Light::intensity. */
	enum class LightIntensityUnit
	{
		Relative,
		Lux,
		Lumens,
		Candela
	};

	struct ShadowSettings
	{
		bool enabled = false;
		float max_distance = 128.0f;
		float depth_bias = 0.005f;
	};

	/** Public light description for scene graphs. */
	struct Light
	{
		LightKind kind = LightKind::Point;
		LightIntensityUnit intensity_unit = LightIntensityUnit::Relative;
		Vec3 position{};
		Vec3 direction{ 0.0f, -1.0f, 0.0f };
		Color color{};
		float intensity = 1.0f;
		float range = 16.0f;
		float inner_cone_degrees = 20.0f;
		float outer_cone_degrees = 30.0f;
		ShadowSettings shadows{};

		[[nodiscard]] static Light Sun(Vec3 direction, float intensity = 1.0f) noexcept;
		[[nodiscard]] static Light Point(Vec3 position, Color color = {}, float intensity = 1.0f, float range = 16.0f) noexcept;
		[[nodiscard]] static Light Spot(Vec3 position, Vec3 direction, Color color = {},
			float intensity = 1.0f, float range = 16.0f,
			float inner_cone_degrees = 20.0f, float outer_cone_degrees = 30.0f) noexcept;
		Light& UseIntensityUnit(LightIntensityUnit unit) noexcept;
		Light& CastShadows(bool enabled = true, float max_distance = 128.0f, float depth_bias = 0.005f) noexcept;
		Light& SpotCone(float inner_degrees, float outer_degrees) noexcept;
	};
}
