#pragma once

#include <filesystem>

namespace ve::engine
{
	struct StaticModelSceneConfiguration
	{
		std::filesystem::path model_path;
		bool visible = true;
	};
}
