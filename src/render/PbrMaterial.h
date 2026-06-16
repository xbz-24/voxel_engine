#pragma once

#include "GraphicsTypes.h"

#include <glm/glm.hpp>

namespace ve::rendering
{
	struct PbrMaterialProperties
	{
		glm::vec3 albedo{ 1.0f, 1.0f, 1.0f };
		float metallic = 0.0f;
		float roughness = 0.8f;
		float ambient_occlusion = 1.0f;
		float emissive_strength = 0.0f;
	};

	struct PbrTextureSet
	{
		TextureHandle albedo = kInvalidTextureHandle;
		TextureHandle normal = kInvalidTextureHandle;
		TextureHandle roughness = kInvalidTextureHandle;
		TextureHandle metallic = kInvalidTextureHandle;
		TextureHandle ambient_occlusion = kInvalidTextureHandle;
		TextureHandle emissive = kInvalidTextureHandle;
	};

	class PbrMaterial
	{
	public:
		/** Creates a default dielectric material. */
		PbrMaterial() = default;

		/** @param properties Physically based scalar/color material values. */
		explicit PbrMaterial(PbrMaterialProperties properties) noexcept;

		/** @param properties Physically based scalar/color material values. */
		void SetProperties(PbrMaterialProperties properties) noexcept;

		/** @param textures Optional texture maps sampled by PBR shaders. */
		void SetTextures(PbrTextureSet textures) noexcept;

		/** @return Physically based scalar/color material values. */
		[[nodiscard]] const PbrMaterialProperties& Properties() const noexcept;

		/** @return Optional texture maps sampled by PBR shaders. */
		[[nodiscard]] const PbrTextureSet& Textures() const noexcept;

		/** @return True when the material should contribute emission. */
		[[nodiscard]] bool IsEmissive() const noexcept;

	private:
		PbrMaterialProperties properties_{};
		PbrTextureSet textures_{};
	};
}
