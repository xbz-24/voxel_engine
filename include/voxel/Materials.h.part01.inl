
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
