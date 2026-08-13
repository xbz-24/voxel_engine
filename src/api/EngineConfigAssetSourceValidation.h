#pragma once

#include "EngineRuntimeBridge.h"

#include <string_view>

namespace voxel::detail::config_validation
{
	void ValidateAssetSource(
		const AssetSource& source,
		std::string_view legacy_path,
		std::string_view asset_kind,
		bool require_existing_files,
		std::vector<std::string>& issues);
}
