#pragma once

#include "voxel/MaterialTypes.h"

#include <string>

namespace voxel
{
	/** @addtogroup voxel_sdk_materials
	 * @{
	 */
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
		float metallic = 0.0f;
		float roughness = 1.0f;
		float emission = 0.0f;
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
	/** @} */
}
