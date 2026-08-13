#pragma once

#include <filesystem>
#include <glm/glm.hpp>
#include <string>

namespace ve::assets
{
	struct ImportedMaterial
	{
		std::string name;
		glm::vec4 base_color{ 1.0f };
		float metallic = 0.0f;
		float roughness = 0.8f;
		std::filesystem::path albedo_texture;
		std::filesystem::path normal_texture;
		std::filesystem::path metallic_roughness_texture;
		std::filesystem::path metallic_texture;
		std::filesystem::path roughness_texture;
		std::filesystem::path occlusion_texture;
		std::filesystem::path emissive_texture;
	};
}
