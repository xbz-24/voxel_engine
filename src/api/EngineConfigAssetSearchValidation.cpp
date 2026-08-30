#include "EngineConfigValidationInternal.h"
#include "EngineConfigFilesystemInspection.h"

#include <span>
#include <string>

namespace voxel::detail::config_validation
{
	void ValidateSearchRoots(std::span<const std::string> search_roots,
		bool require_existing_directories,
		std::vector<std::string>& issues)
	{
		for (const std::string& search_root : search_roots)
		{
			if (search_root.empty())
			{
				issues.push_back("asset search root must not be empty");
			}
			else if (require_existing_directories)
			{
				const FilesystemRequirementResult result = CheckPathIsDirectory(search_root);
				if (result == FilesystemRequirementResult::InspectionFailed)
				{
					issues.push_back("asset search root could not be inspected: " + search_root);
				}
				else if (result == FilesystemRequirementResult::NotSatisfied)
				{
					issues.push_back("asset search root does not exist: " + search_root);
				}
			}
		}
	}
}
