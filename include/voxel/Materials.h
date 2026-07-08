#pragma once

#include "voxel/Camera.h"

#include <string>
#include <vector>

namespace voxel
{
	/** Inclusive numeric range helper used by material validation. */
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

	/** Linear RGBA color with normalized channels. */
	struct Color
	{
		// Color channels use NormalizedFloatRange.
		float r = 1.0f;
		float g = 1.0f;
		float b = 1.0f;
		float a = 1.0f;
	};

	/** Public material description for authored scenes and asset binding. */
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

		/** Creates a material with the provided lookup name. */
		[[nodiscard]] static Material Named(std::string name);

		/** Sets the material base color. */
		Material& BaseColor(Color value) noexcept;

		/** Binds the base-color texture by asset name. */
		Material& Texture(std::string texture_name);

		/** Binds the normal texture by asset name. */
		Material& NormalTexture(std::string texture_name);

		/** Binds the roughness texture by asset name. */
		Material& RoughnessTexture(std::string texture_name);

		/** Binds the metallic texture by asset name. */
		Material& MetallicTexture(std::string texture_name);

		/** Binds the ambient-occlusion texture by asset name. */
		Material& OcclusionTexture(std::string texture_name);

		/** Binds the emissive texture by asset name. */
		Material& EmissiveTexture(std::string texture_name);

		/** Sets normalized metallic strength. */
		Material& Metallic(float value) noexcept;

		/** Sets normalized roughness strength. */
		Material& Roughness(float value) noexcept;

		/** Sets non-negative emissive strength. */
		Material& Emissive(float strength) noexcept;

		/** Marks whether the material should be treated as transparent. */
		Material& Transparent(bool enabled = true) noexcept;
	};

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

	/** Optional shadow-casting request for a light. */
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

		/** Creates a directional light. */
		[[nodiscard]] static Light Sun(Vec3 direction, float intensity = 1.0f) noexcept;

		/** Creates a point light. */
		[[nodiscard]] static Light Point(Vec3 position, Color color = {}, float intensity = 1.0f, float range = 16.0f) noexcept;

		/** Creates a spot light. */
		[[nodiscard]] static Light Spot(Vec3 position,
			Vec3 direction,
			Color color = {},
			float intensity = 1.0f,
			float range = 16.0f,
			float inner_cone_degrees = 20.0f,
			float outer_cone_degrees = 30.0f) noexcept;

		/** Sets the unit used to interpret intensity. */
		Light& UseIntensityUnit(LightIntensityUnit unit) noexcept;

		/** Enables or disables shadow casting with distance and bias controls. */
		Light& CastShadows(bool enabled = true, float max_distance = 128.0f, float depth_bias = 0.005f) noexcept;

		/** Sets inner and outer spot cone angles in degrees. */
		Light& SpotCone(float inner_degrees, float outer_degrees) noexcept;
	};

	/** Scene-wide environmental lighting and fog request. */
	struct Environment
	{
		Color ambient{ 0.35f, 0.38f, 0.44f, 1.0f };
		Color fog{ 0.56f, 0.68f, 0.84f, 1.0f };
		float fog_density = 0.0f;
		float time_of_day = 12.0f;

		/** Sets ambient light color. */
		Environment& Ambient(Color value) noexcept;

		/** Sets fog color and density. */
		Environment& Fog(Color value, float density) noexcept;

		/** Sets authored time of day in hours. */
		Environment& TimeOfDay(float hour) noexcept;
	};

	/** Collection of named materials referenced by authored scene data. */
	struct MaterialLibrary
	{
		std::vector<Material> materials;

		/** Adds or appends a material definition. */
		MaterialLibrary& Add(Material material);

		/** Returns validation messages for invalid material names, ranges, and texture references. */
		[[nodiscard]] std::vector<std::string> Validate() const;
	};
}
