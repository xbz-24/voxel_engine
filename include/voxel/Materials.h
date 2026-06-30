#pragma once

#include "voxel/Camera.h"

#include <string>
#include <vector>

namespace voxel
{
	struct FloatRange
	{
		float minimum = 0.0f;
		float maximum = 1.0f;

		[[nodiscard]] constexpr bool Contains(float value) const noexcept
		{
			return value >= minimum && value <= maximum;
		}
	};

	inline constexpr FloatRange NormalizedFloatRange{ 0.0f, 1.0f };
	inline constexpr float MinimumEmissionStrength = 0.0f;

	struct Color
	{
		// Color channels use NormalizedFloatRange.
		float r = 1.0f;
		float g = 1.0f;
		float b = 1.0f;
		float a = 1.0f;
	};

	struct Material
	{
		std::string name;
		Color base_color{};
		std::string texture;
		std::string normal_texture;
		std::string roughness_texture;
		std::string metallic_texture;
		std::string occlusion_texture;
		std::string emissive_texture;
		float metallic = 0.0f; // NormalizedFloatRange.
		float roughness = 1.0f; // NormalizedFloatRange.
		float emission = 0.0f; // Must be >= MinimumEmissionStrength.
		bool transparent = false;

		[[nodiscard]] static Material Named(std::string name);

		Material& BaseColor(Color value) noexcept;
		Material& Texture(std::string texture_name);
		Material& NormalTexture(std::string texture_name);
		Material& RoughnessTexture(std::string texture_name);
		Material& MetallicTexture(std::string texture_name);
		Material& OcclusionTexture(std::string texture_name);
		Material& EmissiveTexture(std::string texture_name);
		Material& Metallic(float value) noexcept;
		Material& Roughness(float value) noexcept;
		Material& Emissive(float strength) noexcept;
		Material& Transparent(bool enabled = true) noexcept;
	};

	enum class LightKind
	{
		Directional,
		Point,
		Spot
	};

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
		[[nodiscard]] static Light Spot(Vec3 position,
			Vec3 direction,
			Color color = {},
			float intensity = 1.0f,
			float range = 16.0f,
			float inner_cone_degrees = 20.0f,
			float outer_cone_degrees = 30.0f) noexcept;

		Light& UseIntensityUnit(LightIntensityUnit unit) noexcept;
		Light& CastShadows(bool enabled = true, float max_distance = 128.0f, float depth_bias = 0.005f) noexcept;
		Light& SpotCone(float inner_degrees, float outer_degrees) noexcept;
	};

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

	struct MaterialLibrary
	{
		std::vector<Material> materials;

		MaterialLibrary& Add(Material material);
		[[nodiscard]] std::vector<std::string> Validate() const;
	};
}
