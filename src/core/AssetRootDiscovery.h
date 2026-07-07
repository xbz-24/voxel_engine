#pragma once

#include "AssetPaths.h"

#include <filesystem>

namespace ve::assets::detail
{
	[[nodiscard]] std::filesystem::path ResolveRootDirectory(const AssetPathResolveOptions& options);
}
