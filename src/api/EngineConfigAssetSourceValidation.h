#pragma once

#include "voxel/Assets.h"

#include <string>
#include <string_view>
#include <vector>

namespace voxel::detail::config_validation
{
	void ValidateAssetSource(
		const AssetSource& source,
		std::string_view legacy_path,
		std::string_view asset_kind,
		bool require_existing_files,
		std::vector<std::string>& issues);
}
