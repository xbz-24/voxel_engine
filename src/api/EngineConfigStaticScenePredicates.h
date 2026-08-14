#pragma once

#include "voxel/EngineConfig.h"

#include <algorithm>
#include <cctype>
#include <filesystem>
#include <string>

namespace voxel::detail::static_scene::support_detail
{
	[[nodiscard]] inline bool SameColor(Color left, Color right) noexcept
	{
		return left.r == right.r && left.g == right.g &&
			left.b == right.b && left.a == right.a;
	}

	[[nodiscard]] inline bool IsDefaultEnvironment(const Environment& environment) noexcept
	{
		const Environment defaults{};
		return SameColor(environment.ambient, defaults.ambient) &&
			SameColor(environment.fog, defaults.fog) &&
			environment.fog_density == defaults.fog_density &&
			environment.time_of_day == defaults.time_of_day;
	}

	[[nodiscard]] inline bool IsIdentity(const Transform& transform) noexcept
	{
		return transform.position.x == 0.0f && transform.position.y == 0.0f &&
			transform.position.z == 0.0f && transform.rotation.x == 0.0f &&
			transform.rotation.y == 0.0f && transform.rotation.z == 0.0f &&
			transform.scale.x == 1.0f && transform.scale.y == 1.0f &&
			transform.scale.z == 1.0f;
	}

	[[nodiscard]] inline std::filesystem::path EffectivePath(const ModelAsset& model)
	{
		return model.source.location.empty() ? model.path : model.source.location;
	}

	[[nodiscard]] inline bool HasObjExtension(const std::filesystem::path& path)
	{
		std::string extension = path.extension().string();
		std::ranges::transform(extension, extension.begin(), [](unsigned char value) {
			return static_cast<char>(std::tolower(value));
		});
		return extension == ".obj";
	}
}
