#include "EngineConfigValidationInternal.h"

#include <filesystem>
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
			else if (require_existing_directories && !std::filesystem::is_directory(search_root))
			{
				issues.push_back("asset search root does not exist: " + search_root);
			}
		}
	}
}
