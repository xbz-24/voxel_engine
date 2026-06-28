#pragma once

#include "voxel/Camera.h"

#include <string>
#include <vector>

namespace voxel
{
	// TODO: Replace raw floats with documented ranges and validation for authoring tools.
	struct Color
	{
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

	enum class LightKind
	{
		Directional,
		Point,
		Spot
	};

	struct Light
	{
		// TODO: Add shadow settings, spot cone angles, and physically meaningful intensity units.
		LightKind kind = LightKind::Point;
		Vec3 position{};
		Vec3 direction{ 0.0f, -1.0f, 0.0f };
		Color color{};
		float intensity = 1.0f;
		float range = 16.0f;

		[[nodiscard]] static Light Sun(Vec3 direction, float intensity = 1.0f) noexcept;
		[[nodiscard]] static Light Point(Vec3 position, Color color = {}, float intensity = 1.0f, float range = 16.0f) noexcept;
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
	};
}
