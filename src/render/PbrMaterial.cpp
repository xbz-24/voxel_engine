#include "PbrMaterial.h"

namespace ve::rendering
{
	/// Creates a material from physically based scalar/color values.
	PbrMaterial::PbrMaterial(PbrMaterialProperties properties) noexcept
		: properties_(properties)
	{
	}

	/// Replaces the material scalar/color values.
	void PbrMaterial::SetProperties(PbrMaterialProperties properties) noexcept
	{
		properties_ = properties;
	}

	/// Replaces the material texture handles.
	void PbrMaterial::SetTextures(PbrTextureSet textures) noexcept
	{
		textures_ = textures;
	}

	/// Returns physically based scalar/color material values.
	const PbrMaterialProperties& PbrMaterial::Properties() const noexcept { return properties_; }

	/// Returns optional texture maps sampled by PBR shaders.
	const PbrTextureSet& PbrMaterial::Textures() const noexcept { return textures_; }

	/// Reports whether the material should contribute emission.
	bool PbrMaterial::IsEmissive() const noexcept { return properties_.emissive_strength > 0.0f; }
}
