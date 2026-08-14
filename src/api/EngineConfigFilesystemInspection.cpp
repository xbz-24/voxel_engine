#include "EngineConfigFilesystemInspection.h"

#include <filesystem>
#include <stdexcept>
#include <system_error>

namespace voxel::detail::config_validation
{
	namespace
	{
		template <typename Query>
		[[nodiscard]] FilesystemRequirementResult CheckRequirement(
			const std::string& path_text,
			Query query)
		{
			try
			{
				const std::filesystem::path path{ path_text };
				std::error_code error;
				const bool satisfied = query(path, error);
				if (error) return FilesystemRequirementResult::InspectionFailed;
				return satisfied ?
					FilesystemRequirementResult::Satisfied :
					FilesystemRequirementResult::NotSatisfied;
			}
			catch (const std::filesystem::filesystem_error&)
			{
				return FilesystemRequirementResult::InspectionFailed;
			}
			catch (const std::range_error&)
			{
				return FilesystemRequirementResult::InspectionFailed;
			}
		}
	}

	FilesystemRequirementResult CheckPathExists(const std::string& path_text)
	{
		return CheckRequirement(path_text,
			[](const std::filesystem::path& path, std::error_code& error)
			{
				return std::filesystem::exists(path, error);
			});
	}

	FilesystemRequirementResult CheckPathIsDirectory(const std::string& path_text)
	{
		return CheckRequirement(path_text,
			[](const std::filesystem::path& path, std::error_code& error)
			{
				return std::filesystem::is_directory(path, error);
			});
	}
}
