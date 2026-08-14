#pragma once

#include <glm/vec3.hpp>

#include <filesystem>

namespace ve::engine
{
	struct StaticModelSceneConfiguration
	{
		std::filesystem::path model_path;
		bool visible = true;
		glm::vec3 root_translation{ 0.0f };
	};
}
