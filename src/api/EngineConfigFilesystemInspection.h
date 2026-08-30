#pragma once

#include <string>

namespace voxel::detail::config_validation
{
	enum class FilesystemRequirementResult
	{
		Satisfied,
		NotSatisfied,
		InspectionFailed
	};

	[[nodiscard]] FilesystemRequirementResult CheckPathExists(const std::string& path_text);
	[[nodiscard]] FilesystemRequirementResult CheckPathIsDirectory(const std::string& path_text);
}
