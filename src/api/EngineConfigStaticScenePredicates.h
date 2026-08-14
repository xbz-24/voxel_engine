#pragma once

#include "voxel/EngineConfig.h"

#include <algorithm>
#include <cctype>
#include <cmath>
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

	[[nodiscard]] inline bool IsFinite(Vec3 value) noexcept
	{
		return std::isfinite(value.x) && std::isfinite(value.y) && std::isfinite(value.z);
	}

	[[nodiscard]] inline bool IsZero(Vec3 value) noexcept
	{
		return value.x == 0.0f && value.y == 0.0f && value.z == 0.0f;
	}

	[[nodiscard]] inline bool IsUnitScale(Vec3 value) noexcept
	{
		return value.x == 1.0f && value.y == 1.0f && value.z == 1.0f;
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
